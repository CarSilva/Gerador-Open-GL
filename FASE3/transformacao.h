#ifndef TRANSFORMACAO_H
#define TRANSFORMACAO_H
;

class Transformacao {
	public:
		virtual void transformar() = 0;
};

class PopMatrix: public Transformacao {
	public:
		virtual void transformar(){
			glPopMatrix();
		}
};

class PushMatrix: public Transformacao {
	public:
		virtual void transformar(){
			glPushMatrix();
		}
};

class Rotate: public Transformacao {
	public:
		float x, y, z, angle;
		float time;
		bool usetime;
		Rotate(float ang, float xx, float yy, float zz){
		 angle = ang;
		 x = xx;
		 y = yy;
		 z = zz;
		 usetime = false;
		}
		Rotate(float tt, float xx, float yy, float zz, bool ut) {
			time = tt;
			x = xx;
			y = yy;
			z = zz;
			usetime = true;
		}
		virtual void transformar() {
			if(usetime)
				angle = fmod((glutGet(GLUT_ELAPSED_TIME) * 360.0f) / (time * 1000.0f), 360.0f);
			glRotatef(angle,x,y,z);
			glutPostRedisplay();
		}
};

class Translate: public Transformacao {
	public:
		float x,y,z;
		std::vector<float> points;
		float time;
		bool usetime;
		Translate(float xx,float yy,float zz) {
			x = xx;
			y = yy;
			z = zz;
			usetime = false;
		}
		Translate(float tt, std::vector<float> pp) {
			time = tt;
			points = pp;
			usetime = true;
		}
		virtual void transformar(){
			if(usetime) {
				float res[3];
				float der[3];
				float z[3];
				float up[3];
				// -------------------------------------------------
				// Tratar da transformacao com as curvas em "points"
				// -------------------------------------------------
				renderCatmullRomCurve();
				getGlobalCatmullRomPoint(time, res, der);
				glTranslatef(res[0], res[1], res[2]);
				cross(der,up,z);
				cross(z,der,up);
				normalize(der); normalize(up); normalize(z);
				float m[4][4] = { {der[0], der[1], der[2], 0},
							      			{ up[0], up[1], up[2], 0},
							      			{ z[0], z[1], z[2], 0},
							      			{ 0, 0, 0, 1} };
				glMultMatrixf((float *) m);
				time += 0.001f;
				//glutPostRedisplay();
			}
			else glTranslatef(x, y, z);
		}

		void cross(float *a, float *b, float *res) {
			res[0] = a[1]*b[2] - a[2]*b[1];
			res[1] = a[2]*b[0] - a[0]*b[2];
			res[2] = a[0]*b[1] - a[1]*b[0];
		}

		void normalize(float *a) {
			float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
			a[0] = a[0]/l;
			a[1] = a[1]/l;
			a[2] = a[2]/l;
		}

		void multMatrixVector(float *m, float *v, float *res) {
			for (int j = 0; j < 4; ++j) {
				res[j] = 0;
				for (int k = 0; k < 4; ++k) res[j] += v[k] * m[j * 4 + k];
			}
		}

		void getCatmullRomPoint(float t, float *p0, float *p1, float *p2, float *p3, float *res, float *deriv){
			float px[4], py[4], pz[4], ax[4], ay[4], az[4];
			px[0] = p0[0]; px[1] = p1[0]; px[2] = p2[0]; px[3] = p3[0];
			py[0] = p0[1]; py[1] = p1[1]; py[2] = p2[1]; py[3] = p3[1];
			pz[0] = p0[2]; pz[1] = p1[2]; pz[2] = p2[2]; pz[3] = p3[2];
			float tGrande[4] = {t*t*t, t*t, t, 1};
			float tDer[4] = {3*t*t, 2*t, 1, 0};

			// catmull-rom matrix
			float m[4][4] = {	{-0.5f,  1.5f, -1.5f,  0.5f},
												{ 1.0f, -2.5f,  2.0f, -0.5f},
												{-0.5f,  0.0f,  0.5f,  0.0f},
												{ 0.0f,  1.0f,  0.0f,  0.0f} };

			// reset res and deriv
			res[0] = 0.0; res[1] = 0.0; res[2] = 0.0;
			deriv[0] = 0.0; deriv[1] = 0.0; deriv[2] = 0.0;

			// Compute A = M * P
			multMatrixVector((float*) m,px,ax);
			multMatrixVector((float*) m,py,ay);
			multMatrixVector((float*) m,pz,az);

			// Compute point res = T *A
			res[0]= tGrande[0] * ax[0] + tGrande[1] * ax[1] + tGrande[2] * ax[2] + tGrande[3] * ax[3];
			res[1]= tGrande[0] * ay[0] + tGrande[1] * ay[1] + tGrande[2] * ay[2] + tGrande[3] * ay[3];
			res[2]= tGrande[0] * az[0] + tGrande[1] * az[1] + tGrande[2] * az[2] + tGrande[3] * az[3];

			// compute deriv = T' * A
			deriv[0]= tDer[0] * ax[0] + tDer[1] * ax[1] + tDer[2] * ax[2] + tDer[3] * ax[3];
			deriv[1]= tDer[0] * ay[0] + tDer[1] * ay[1] + tDer[2] * ay[2] + tDer[3] * ay[3];
			deriv[2]= tDer[0] * az[0] + tDer[1] * az[1] + tDer[2] * az[2] + tDer[3] * az[3];
			// ...
		}

		// given  global t, returns the point in the curve
		void getGlobalCatmullRomPoint(float gt, float *res, float *deriv) {
			float t = gt * points.size(); // this is the real global t
			int index = floor(t);  // which segment
			t = t - index; // where within  the segment

			// indices store the points
			int indices[4];
			indices[0] = (index + points.size()-1)%points.size();
			indices[1] = (indices[0]+1)%points.size();
			indices[2] = (indices[1]+1)%points.size();
			indices[3] = (indices[2]+1)%points.size();

			getCatmullRomPoint(t, &points[indices[0]],
														&points[indices[1]],
														&points[indices[2]],
														&points[indices[3]], res, deriv);
		}


		void renderCatmullRomCurve() {
			float res[3], der[3];
			glBegin(GL_LINE_LOOP);
				for( int i=0; i<100; i++ ){
					getGlobalCatmullRomPoint((float)i/100,res,der);
					glVertex3f(res[0], res[1], res[2]);
				}
			glEnd();
		}
};

class Scale: public Transformacao {
	public:
		float x,y,z;
		Scale(float xx,float yy,float zz){
			x = xx;
			y = yy;
			z = zz;
		}
		virtual void transformar(){
			glScalef(x, y, z);
		}
};

class Model: public Transformacao {
	public:
		Model(std::vector<float> v){
			vc = v;
		}
		std::vector<float> vc;
		virtual void transformar(){
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vc.size(), &vc[0], GL_STATIC_DRAW);
			glVertexPointer(3,GL_FLOAT,0,0);
			glDrawArrays(GL_TRIANGLES, 0, vc.size());
		}
};

class Color: public Transformacao {
	public:
		float r,g,b;
		Color(float red, float green, float blue){
			r = red;
			g = green;
			b = blue;
		}
		virtual void transformar(){
			glColor3ub(r, g, b);
		}
};



#endif
