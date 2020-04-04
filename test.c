#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "worker.h"

Image *read_image(char *filename);

int main(){

Image *bob;

bob = read_image("bob.ppm");
//print_image(bob);

//printf("%f",compare_images(bob,"steve.ppm"));




CompRecord c = process_dir("./BOB",bob,5);













}