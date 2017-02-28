#include <math.h>
#include <stdio.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdlib.h>
#include <string.h>


void plane(float x, float z, char* ficheiro);
void box(float x, float y, float z, int stacks, int slices, char* ficheiro);
void cone (float r, float h, float sl, float st, char *ficheiro);



int main(int argc, char **argv) {
	int err = 0;
	if(argc < 2){
		printf("Input inválido.\n");
		exit(-1);
	}

	if(strcmp(argv[1], "plane") == 0)
			plane(atof(argv[2]), atof(argv[3]), argv[4]);
	else if(strcmp(argv[1], "box") == 0)
			box(atof(argv[2]), atof(argv[3]), atof(argv[4]), atoi(argv[5]), atoi(argv[6]), argv[7]);
	else if(strcmp(argv[1], "cone") == 0)
			cone(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), argv[6]);
	else printf("Not suported yet!");

	return 1;
}



void plane(float x, float z, char* ficheiro){
	int r;
	FILE *op;
	op = fopen(ficheiro, "w+");
	if (op < 0) {
		printf("Unable to open %s.", ficheiro);
		return;
	}

	fprintf(op, "%f 0 %f\n", x/2, z/2);
	fprintf(op, "%f 0 %f\n", x/2, -z/2);
	fprintf(op, "%f 0 %f\n", -x/2, -z/2);
	fprintf(op, "%f 0 %f\n", -x/2, -z/2);
	fprintf(op, "%f 0 %f\n", -x/2, z/2);
	fprintf(op, "%f 0 %f\n", x/2, z/2);
	fclose(op);
}



void box(float x, float y, float z, int stacks, int slices, char* ficheiro){
	int r,st,sl;
	float yaux0,yaux1,zaux0,zaux1;
	//altura das stacks
	float yaux = y / stacks;
	//largura das slices
	float zaux = z / slices;
	//coordenadas negativas do z
	float zn = z/2 - z;
	//coordenadas negativas do y
	float yn = y/2 - y;
	//coordenadas negativas do x
	float xn = x/2 - x;
	x=x/2;
	y=y/2;
	z=z/2;
	FILE *op = fopen(ficheiro, "w+");

	for(sl = 1; sl <= slices; sl++) {
		zaux0 = z - (sl-1) * zaux;
		zaux1 = z - sl * zaux;

		//face de baixo
		fprintf(op, "%f %f %f\n",xn, yn, zaux0);
		fprintf(op, "%f %f %f\n",xn, yn, zaux1);
		fprintf(op, "%f %f %f\n", x, yn, zaux1);
		fprintf(op, "%f %f %f\n", x, yn, zaux1);
		fprintf(op, "%f %f %f\n", x, yn, zaux0);
		fprintf(op, "%f %f %f\n",xn, yn, zaux0);

		//face de cima
		fprintf(op, "%f %f %f\n", x, y, zaux0);
		fprintf(op, "%f %f %f\n", x, y, zaux1);
		fprintf(op, "%f %f %f\n",xn, y, zaux1);
		fprintf(op, "%f %f %f\n",xn, y, zaux1);
		fprintf(op, "%f %f %f\n",xn, y, zaux0);
		fprintf(op, "%f %f %f\n", x, y, zaux0);
	}

	for(st = 1; st <= stacks; st++) {
		yaux0 = y - (st-1) * yaux;
		yaux1 = y - st * yaux;
		//face da frente
		fprintf(op, "%f %f %f\n", x, yaux0, z);
		fprintf(op, "%f %f %f\n",xn, yaux0, z);
		fprintf(op, "%f %f %f\n",xn, yaux1, z);
		fprintf(op, "%f %f %f\n",xn, yaux1, z);
		fprintf(op, "%f %f %f\n", x, yaux1, z);
		fprintf(op, "%f %f %f\n", x, yaux0, z);
		//face de traz
		fprintf(op, "%f %f %f\n",xn, yaux1, zn);
		fprintf(op, "%f %f %f\n",xn, yaux0, zn);
		fprintf(op, "%f %f %f\n", x, yaux0, zn);
		fprintf(op, "%f %f %f\n",xn, yaux1, zn);
		fprintf(op, "%f %f %f\n", x, yaux0, zn);
		fprintf(op, "%f %f %f\n", x, yaux1, zn);

		for(sl = 1; sl <= slices; sl++) {
			zaux0 = z - (sl-1) * zaux;
			zaux1 = z - sl * zaux;
			//face da direita
			fprintf(op, "%f %f %f\n", x, yaux1, zaux1);
			fprintf(op, "%f %f %f\n", x, yaux0, zaux1);
			fprintf(op, "%f %f %f\n", x, yaux1, zaux0);
			fprintf(op, "%f %f %f\n", x, yaux1, zaux0);
			fprintf(op, "%f %f %f\n", x, yaux0, zaux1);
			fprintf(op, "%f %f %f\n", x, yaux0, zaux0);

			//face da esquerda
			fprintf(op, "%f %f %f\n",xn, yaux0, zaux1);
			fprintf(op, "%f %f %f\n",xn, yaux1, zaux1);
			fprintf(op, "%f %f %f\n",xn, yaux1, zaux0);
			fprintf(op, "%f %f %f\n",xn, yaux1, zaux0);
			fprintf(op, "%f %f %f\n",xn, yaux0, zaux0);
			fprintf(op, "%f %f %f\n",xn, yaux0, zaux1);
		}
	}
	fclose(op);
}



void cone (float r, float h, float sl, float st, char *ficheiro) {
	int i, j;
	float y;
	FILE *op = fopen(ficheiro, "w+");
	if (op < 0) {
		printf("Unable to open %s.", ficheiro);
		return;
	}

	for(i = 1; i <= sl; i++) {
		//faces
		fprintf(op,"%f 0 %f\n", r*cos(i*2*M_PI/sl), r*sin(i*2*M_PI/sl));
		fprintf(op,"%f 0 %f\n", r*cos((i-1)*2*M_PI/sl), r*sin((i-1)*2*M_PI/sl));
		fprintf(op,"0 %f 0\n", h);
	}
	for (j = 0; j < st; j++) {
		y = ((h - ((h / st) * j)) * r) / h;
		for(i = 1; i <= sl; i++) {
			//bases
			fprintf(op,"0 %f 0\n", (h/st)*j);
			fprintf(op,"%f %f %f\n", y*cos((i-1)*2*M_PI/sl), (h/st)*j, y*sin((i-1)*2*M_PI/sl));
			fprintf(op,"%f %f %f\n", y*cos(i*2*M_PI/sl), (h/st)*j , y*sin(i*2*M_PI/sl));
		}
	}
	fclose(op);
}



/*
void cone (float r, float h, float sl, char *ficheiro) {
	FILE *op = fopen(ficheiro, "w+");
	int i;
	if (op < 0) {
		printf("Unable to open %s.", ficheiro);
		return;
	}

	for(i = 1; i <= sl; i++){
		// base
		fprintf(op,"0 0 0\n");
		fprintf(op,"%f 0 %f\n", r*cos(i*2*M_PI/sl), r*sin(i*2*M_PI/sl));
		fprintf(op,"%f 0 %f\n", r*cos((i-1)*2*M_PI/sl), r*sin((i-1)*2*M_PI/sl));

		// faces
		fprintf(op,"%f 0 %f\n", r*cos(i*2*M_PI/sl), r*sin(i*2*M_PI/sl));
		fprintf(op,"0 %f 0\n", h);
		fprintf(op,"%f 0 %f\n", r*cos((i-1)*2*M_PI/sl), r*sin((i-1)*2*M_PI/sl));
	}
	fclose(op);
}
*/
