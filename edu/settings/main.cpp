//
// Install libyaml
//  sudo apt install libyaml-dev

#include <stdio.h>
#include "settings.h"

int main(void)
{
Settings s;
	if(s.load("config.yaml") != 0) {
		printf("can\'t load settings\n");
		exit(1);
    }
	printf("width = %s\n", s.get("width").c_str());
	printf("idth = %s\n", s.get("idth").c_str());

    return 0;
}
