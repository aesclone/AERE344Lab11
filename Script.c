#include "csv.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

struct data{

  double*** vel;
  int rows, columns, sets;
  double dx, dy;

};

double *get_double(const char *str){

  double *num = (double *) malloc(sizeof(double));
  int dec = -1;
  int pos = 1;
  int index  = 0;
  int len = strlen(str);

  if(len < 1){ return NULL; }

  if(str[0] == '-'){

    pos = -1;
    index += 1;

  }

  while(index < len && str[index] != '.'){

    if(isdigit(str[index]) == 0){ return NULL; }

    *num = (*num * 10) + str[index] - '0';
    index += 1;

  }

  if(str[index] == '.'){ index += 1; }

  while(index < len && str[index] != 'e'){

    if(isdigit(str[index]) == 0){ return NULL; }

    *num = *num + ((str[index] - '0') * pow(10, dec--));
    index += 1;

  }

  if(index + 1 == len){ return NULL; }

  if(str[index] == 'e'){

    index += 1;

    int power = 0;
    int epos = 1;
    if(str[index] == '-'){

      epos = -1;
      index += 1;

    }

    if(index == len){ return NULL; }

    while(index < len){

      if(isdigit(str[index]) == 0){ return NULL; }

      power = (power * 10) + str[index] - '0';
      index += 1;

    }

    *num = pow(*num, power);

  }

  *num = *num * pos;

  return num;

}

char *get_size(char *filename, char *delimeter, int header, int x, int y,  int *rows, int *columns, double *hx, double *hy){

  CsvParser *data = CsvParser_new(filename, delimeter, header);
  if(data == NULL){ printf("CsvParser failed to create!\n"); return NULL; }


  CsvRow *row = CsvParser_getRow(data);
  if(CsvParser_getNumFields(row) != 4){

    printf("Not the proper number of columns in the file!\n");
    CsvParser_destroy(data);
    return NULL;
  }

  const char **rowFields = CsvParser_getFields(row);
  double *xprev = get_double(rowFields[x]);
  if(xprev == NULL){

    printf("Invalid double \"%s\"\n", rowFields[x]);
    free(xprev);
    CsvParser_destroy_row(row);
    return NULL;

  }
  double *yprev = get_double(rowFields[y]);
  if(yprev == NULL){

    printf("Invalid double \"%s\"\n", rowFields[y]);
    free(yprev);
    free(xprev);
    CsvParser_destroy_row(row);
    CsvParser_destroy(data);
    return NULL;

  }

  CsvParser_destroy_row(row);
  *rows = 1;
  int total = 1;
  double xmax = *xprev, xmin = *xprev;
  free(xprev);
  double ymax = *yprev, ymin = *yprev;
  while((row = CsvParser_getRow(data))){

    if(CsvParser_getNumFields(row) != 4){

      printf("Not the proper number of columns in the file!\n");
      free(xprev);
      free(yprev);
      CsvParser_destroy_row(row);
      CsvParser_destroy(data);
      return NULL;

    }

    rowFields = CsvParser_getFields(row);
    double *xcurr = get_double(rowFields[x]);
    double *ycurr = get_double(rowFields[y]);
    if(xcurr == NULL){

      printf("Invalid double \"%s\"\n", xcurr);
      free(xcurr);
      free(ycurr);
      free(yprev);
      CsvParser_destroy_row(row);
      CsvParser_destroy(data);
      return NULL;

    }

    if(ycurr == NULL){

      printf("Invalid double \"%s\"\n", ycurr);
      free(xcurr);
      free(ycurr);
      free(yprev);
      CsvParser_destroy_row(row);
      CsvParser_destroy(data);
      return NULL;

    }

    if(*xcurr > xmax){ xmax = *xcurr;
    }else if(*xcurr < xmin){ xmin = *xcurr; }

    if(*ycurr != *yprev){

      *rows = *rows + 1;
      if(*ycurr > ymax){ ymax = *ycurr;
      }else if(*ycurr < ymin){ ymin = *ycurr; }

    }

    yprev = ycurr;
    total++;
    CsvParser_destroy_row(row);

  }

  CsvParser_destroy(data);
  free(yprev);

  *columns = total / *rows;

  *hx = fabs((xmax - xmin) / *columns);
  *hy = fabs((ymax - ymin) / *rows);

  char *ret = (char *)malloc(sizeof(char));
  *ret = 0;

  return ret;

}

double **get_matrix(char *filename, char *delimeter, int header, int n, int m, int column){

  CsvParser *data = CsvParser_new(filename, delimeter, header);
  if(data == NULL){

    printf("CsvParser failed to create!\n");
    CsvParser_destroy(data);
    return NULL;

  }

  CsvRow *row;
  int i, j;
  double **arr = (double **) malloc(sizeof(double *) * m);
  for(i = 0; i < m; i++){

    arr[i] = (double *) malloc(sizeof(double) * n);
    for(j = 0; j < n; j++){

      row = CsvParser_getRow(data);
      const char **rowFields = CsvParser_getFields(row);
      double *test = get_double(rowFields[column]);
      if(test == NULL){

	printf("Invalid number \"%s\"!\n", rowFields[column]);
	CsvParser_destroy_row(row);
	CsvParser_destroy(data);
	int k;
	for(k = 0; k < j; k++){ free(arr[k]); }
	free(arr);
	free(test);
	return NULL;

      }

      arr[i][j] = *test;
      free(test);
      CsvParser_destroy_row(row);

    }

  }

  CsvParser_destroy(data);

  return arr;

}

char** fileList(char* directory, int* num){

  int numFiles = 0;
  DIR* d;
  struct dirent* dir;
  d = opendir(directory);
  if(d){

    while((dir = readdir(d))){

      if(dir->d_name[0] != '.'){ numFiles++; }

    }
    closedir(d);

  }else{ return NULL; }

  *num = numFiles;

  char** files = (char **)malloc(sizeof(char *) * numFiles);
  int dirLen = strlen(directory);

  d = opendir(directory);
  if(d){

    numFiles = 0;
    int dirLen = strlen(directory);
    while((dir = readdir(d)) != NULL){

      if(dir->d_name[0] != '.'){

	int fileLen = strlen(dir->d_name);
	files[numFiles] = (char *)malloc(sizeof(char) * (fileLen + dirLen + 1));
	int i;
	for(i = 0; i < dirLen; i++){ files[numFiles][i] = directory[i]; }
	files[numFiles][dirLen] = '/';
	for(i = 0; i < fileLen; i++){

	  files[numFiles][i + dirLen + 1] = dir->d_name[i];

	}

	numFiles++;

      }

    }
    closedir(d);

  }else{ return NULL; }

  return files;

}

void  matrixList(char** files, int numFiles, char *del, int header, int col, struct data* d){

  d->vel = (double ***)malloc(sizeof(double **) * numFiles);
  char* size = get_size(files[0], del, header, 0, 1, &d->rows, &d->columns, &d->dx, &d->dy);

  d->sets = numFiles;

  int i;
  for(i = 0; i < numFiles; i++){

    d->vel[i] = get_matrix(files[i], del, header, d->columns, d->rows, col);

  }

}

double vInfinity(struct data* X, int pos){

  double v = 0;
  int i, j;
  double* avg = (double *)malloc(sizeof(double) * X->sets);
  for(i = 0; i < X->sets; i++){

    for(j = 0; j < X->columns; j++){ avg[i] = avg[i] + X->vel[i][pos][j]; }
    avg[i] = avg[i] / X->columns;

  }

  for(i = 0; i < X->sets; i++){
    
    v = v + avg[i];

  }

  free(avg);

  return v / X->columns;

}

double** turbulenceKineticEnergy(struct data* X, struct data* Y, int velInf, int set){

  double** tke = (double **)malloc(sizeof(double *) * X->rows);
  double** U = (double **)malloc(sizeof(double *) * X->rows);
  double** V = (double **)malloc(sizeof(double *) * X->rows);
  int i, j, k;
  for(i = 0; i < X->rows; i++){

    tke[i] = malloc(sizeof(double) * X->columns);
    U[i] = malloc(sizeof(double) * X->columns);
    V[i] = malloc(sizeof(double) * X->columns);

  }

  double u, v;
  for(i = 0; i < X->rows; i++){

    for(j = 0; j < X->columns; j++){

      u = 0;
      v = 0;

      for(k = 0; k < X->sets; k++){
	
	u = u + X->vel[k][i][j];
	v = v + Y->vel[k][i][j];

      }

      U[i][j] = u / X->sets;
      V[i][j] = v / Y->sets;

    }

  }

  for(i = 0; i < X->rows; i++){

    for(j = 0; j < X->columns; j++){

      tke[i][j] = ((pow(X->vel[set][i][i] - U[i][j], 2)) + (pow(Y->vel[set][i][j] - V[i][j] , 2))) / (2 * pow(velInf, 2));

    }

  }

  return tke;

}

void writeTKEFile(char* name, double** tke, int n, int m){

  FILE *file = fopen(name, "w");
  int i, j;
  for(i = 0; i < m; i++){

    for(j = 0; j < n; j++){

      if(tke[i][j] > 1){ fprintf(file, "0.0 ");

      }else{ fprintf(file, "%lf ", tke[i][j]); }

    }

    fprintf(file, "\n");

  }

  fclose(file);
  
}

void smoothData(double** set, int* factors, int size, int rows, int columns){

  int i, j;
  double** temp = malloc(sizeof(double *) * rows);
  for(i = 0; i < columns; i++){

    temp[i] = (double *)malloc(sizeof(double) * columns);

  }

  for(i = 0; i < rows; i++){

    for(j = 0; j < columns; j++){ temp[i][j] = set[i][j]; }

  }
  
  int k, l, m, count;
  double avg;
  for(i = 0; i < size; i++){

    for(j = factors[i]; j < rows - factors[i]; j++){

      for(k = factors[i]; k < columns - factors[i]; k++){

	avg = 0;
	count = 0;

	for(l = j - factors[i]; l < j + factors[i]; l++){

	  for(m = k - factors[i]; m < k + factors[i]; m++){

	    avg = avg + set[l][m];
	    count++;

	  }

	}

	temp[j][k] = avg / count;

      }

    }

    for(j = 0; j < rows; j++){

      for(k = 0; k < columns; k++){

	if(j > factors[i] && j < rows - factors[i] && k > factors[i] && k < columns - factors[i]){

	  if(fabs(set[j][k]) > fabs(2 * temp[j][k])){

	    avg = 0;
	    count = 0;

	    for(l = j - factors[i]; l < j + factors[i]; l++){

	      for(m = k - factors[i]; m < k + factors[i]; m++){

		if(!(l == j && m == k)){

		  avg = avg + set[l][m];
		  count++;

		}

	      }

	    }

	    printf("old: %lf, new: %lf\n", set[j][k], avg / count);

	    set[j][k] = avg / count;

	  }else{ set[j][k] = temp[j][k]; }

	}else{ set[j][k] = temp[j][k]; }

      }

    }

  }

}

double* wakeDistribution(struct data* set, int loc, double vInf){

  double* wake = (double *)malloc(sizeof(double) * set->rows);

  int i, j, count;
  double avg;
  for(i = 0; i < set->rows; i++){

    avg = 0;
    count = 0;
    for(j = 0; j < set->sets; j++){

      avg = avg + set->vel[j][i][loc];
      count++;

    }

    wake[i] = pow(avg / count, 2) / pow(vInf, 2);

  }

  return wake;

}

void writeWakeDistr(char* name, double* data, int size, double dy){

  FILE* file = fopen(name, "w");
  if(file == NULL){

    printf("Could not open Wake file!\n");
    exit(-1);

  }

  int i, back = size - 1;
  for(i = 0; i < size; i++){

    fprintf(file, "%lf %lf\n", data[i], (back * dy) / 1000);
    back--;

  }
  fclose(file);

}

int main(int argc, char **argv){

  if(argc < 2){

    printf("No input directory given!\n");
    return -1;

  }

  int numFiles;
  char** files = fileList(argv[1], &numFiles);
  if(files == NULL){

    printf("Getting files failed!\n");
    return -1;

  }

  struct data xVal;
  struct data yVal;
  matrixList(files, numFiles, "\t", 1, 2, &xVal);
  matrixList(files, numFiles, "\t", 1, 3, &yVal);
  int i;
  for(i = 0; i < numFiles; i++){ free(files[i]); }
  free(files);

  double vInf = vInfinity(&xVal, (int)((5 * xVal.rows) / 6));
  double** tke = turbulenceKineticEnergy(&xVal, &yVal, vInf, 0);

  int factors[6] = {2, 3, 5, 5, 3, 2};
  
  writeTKEFile("TKE16.dat", tke, xVal.columns, xVal.rows);
  free(tke);

  double* wake = wakeDistribution(&xVal, (int)(xVal.columns / 3), vInf);
  writeWakeDistr("WakeDist16.dat", wake, xVal.rows, xVal.dy);
  free(wake);

  system("gnuplot heatPlot.gp");

  return 0;

}
