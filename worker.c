#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <math.h>
#include <float.h>
#include "worker.h"


/*
 * Read an image from a file and create a corresponding 
 * image struct 
 */

Image* read_image(char *filename)
{
        Image *img;
		
		FILE *image_file;

image_file = fopen(filename, "r");
		if(image_file == NULL){
			
			return NULL; 
		}
		char ppm[3];
		ppm[0] = getc(image_file);
		ppm[1] = getc(image_file);
		ppm[2] = getc(image_file);
		ppm[3] = '\0';

if(strcmp(ppm, "P3\n") != 0 && strcmp(ppm, "P3 ")!= 0 && strcmp(ppm, "P3\t")!= 0){

return NULL;
}

char c; 
while(1){
	c = getc(image_file);
	if(c != ' ' && c != '\t' && c != '\n' ) {
	break; 
	}
}

char width[25];
width[0] = c;

for(int i = 1; i<25; i++){
	
	c = getc(image_file);
	
	if(c>= '0' && c<='9'){
		width[i] = c;
	}else{
		width[i] = '\0';
		break; 
	}
}

int x; 
sscanf(width, "%d", &x); 


while(1){
	c = getc(image_file);
	if(c != ' ' && c != '\t' && c != '\n' ) {
	break; 
	}
}
char height[25];
height[0] = c;
int y; 
for(int i = 1; i<25; i++){
	
	c = getc(image_file);
	
	if(c>= '0' && c<='9'){
		height[i] = c;
	}else{
		height[i] = '\0';
		break; 
	}
}
sscanf(height, "%d", &y);

while(1){
	c = getc(image_file);
	if(c != ' ' && c != '\t' && c != '\n' ) {
	break; 
	}
}
char max_val[25];

max_val[0] = c;
int z; 
for(int i = 1; i<25; i++){
	
	c = getc(image_file);
	
	if(c>= '0' && c<='9'){
		max_val[i] = c;
		
	}else{
		max_val[i] = '\0';
		
		break; 
	}
}


sscanf(max_val, "%d", &z);
img = (Image *)malloc(1*sizeof(Image));
img->width=x;
img->height=y;
img->max_value = z;



Pixel *p;
p = (Pixel *)malloc(x*y*sizeof(Pixel));


int p_count = 0;
char line[70];

	
while(fgets(line, 71, image_file) != NULL){
	int red, blue, green;
	char val[70];
	int first = 1;
	int index = 0;
	int counter = 0;

	for(int i = 0; i<70;i++){
		
		if(line[i] == '\0' && !first){
			//insert pixel
				val[index] = '\0';
				index = 0;
				sscanf(val,"%d",&blue);
				counter= 0;
				
				p[p_count].red = red;
				p[p_count].blue = blue;
				p[p_count].green = green;
				
				p_count++;
				
			break;
		}
		
		if(line[i]>= '0' && line[i]<= '9' && first){
			val[index] = line[i];
			index++;
			first = 0;
			
		}else if(line[i]>= '0' && line[i]<= '9' && !first){
			val[index] = line[i];
			index++;
			
		}else if(!first){
			if(counter == 0){
				val[index] = '\0';
				index = 0;
				sscanf(val,"%d",&red);
				counter++;
			}else if(counter == 1){
				val[index] = '\0';
				index = 0;
				sscanf(val,"%d",&green);
				counter++;
				
			}else{
				val[index] = '\0';
				index = 0;
				sscanf(val,"%d",&blue);
				counter= 0;
				
				p[p_count].red = red;
				p[p_count].blue = blue;
				p[p_count].green = green;
				
				p_count++;				
			}			
			first = 1;
		}	
	}
}
img->p = p;


fclose(image_file);
       return img;
}

/*
 * Print an image based on the provided Image struct 
 */
void print_image(Image *img){        
printf("P3\n");        
printf("%d %d\n", img->width, img->height);        
printf("%d\n", img->max_value);        
for(int i=0; i<img->width*img->height; i++)           
	printf("%d %d %d  ", img->p[i].red, img->p[i].green, img->p[i].blue);        
printf("\n");
}


/* * Compute the Euclidian distance between two pixels  
*/
float eucl_distance (Pixel p1, Pixel p2) 
{        
return sqrt( pow(p1.red - p2.red,2 ) + pow( p1.blue - p2.blue, 2) + pow(p1.green - p2.green, 2));
}
/* * Compute the average Euclidian distance between the pixels  
* in the image provided by img1 and the image contained in 
* the file filename 
*/
float compare_images(Image *img1, char *filename) 
{      
	Image *img2;
	img2 = read_image(filename);
	
	if(img1->width!=img2->width || img1->height!=img2->height){
		return FLT_MAX;
	}
	
	float num_pixels = img1->width*img1->height;
	float accumulator = 0;
	for(int i = 0; i<num_pixels; i++){
		accumulator+=eucl_distance(img1->p[i],img2->p[i]);
		
		
		
	}
	
	
	
	
 return accumulator/num_pixels;
}
/* process all files in one directory and find most similar image among them
* - open the directory and find all files in it 
* - for each file read the image in it 
* - compare the image read to the image passed as parameter 
* - keep track of the image that is most similar
 * - write a struct CompRecord with the info for the most similar image to out_fd
*/
CompRecord process_dir(char *dirname, Image *img, int out_fd)
{    
float *distance; 
float closest;
distance = &closest;   
DIR *dir;
dir = opendir(dirname);

if(dir == NULL){
	fprintf(stderr, "Unable to Access Directory\n");
	exit(1);
	
}
//Image *closest = NULL;
*distance = FLT_MAX;
struct dirent *d;

while((d=readdir(dir))!=NULL){
	Image *read = read_image(d->d_name);
	if(read!=NULL){
		if(compare_images(img,d->d_name)<*distance){
			
			
			*distance = compare_images(img,d->d_name);
				//printf("%f",distance);
			//closest = read;
		}
	}
	
	/*
	if(read!= NULL){
		if(compare_images(img,d->d_name)<distance){
			distance = compare_images(img,d->d_name);
			closest = read;
		}
		
	}
*/
}
printf("%f", *distance);
if(img == NULL){
	//printf("%f",FLT_MAX);
		printf("assdsada");
	
}else{
	
	printf("%f", distance);
}

closedir(dir);
CompRecord CRec;        
return CRec;
}





