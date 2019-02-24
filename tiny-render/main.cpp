//
//  main.cpp
//  tiny-render
//
//  Created by Mykhailo Petrenko on 2/13/19.
//  Copyright © 2019 Mykhailo Petrenko. All rights reserved.
//
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>
#include "tgaimage.h"
#include "model.h"

#define IMAGE_HEIGHT 6000
#define IMAGE_WIDTH 6000
#define APEX_COUNT 3

const int IMAGE_X_MIDDLE = IMAGE_WIDTH / 2.;
const int IMAGE_Y_MIDDLE = IMAGE_HEIGHT / 2.;

Model *model = NULL;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage & image, TGAColor color) {
    bool steep = false;
    
    if (abs(x1 - x0) < abs(y1 - y0)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    
    if (x1 < x0) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int kY = (y1 > y0) ? 1 : -1;
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    
    float dInc = abs(dy/(float)dx);
    float inc = 0;
    int y = y0;
    
    for (float x = x0; x < x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        
        inc += dInc;
        
        if (inc > .5) {
            y += kY;
            inc -= 1.;
        }
    }
}

Vec3i transpose(Vec3f v) {
    Vec3f r;
    
    r.x = (int)((v.x + 1.) * IMAGE_X_MIDDLE);
    r.y = (int)((v.y + 1.) * IMAGE_Y_MIDDLE);
    
    return r;
}

bool out_of_bounds(Vec3i v) {
    return (v.x < 0) || (v.y < 0) || (v.x > IMAGE_WIDTH) || (v.y > IMAGE_HEIGHT);
}

int main(int argc, const char * argv[]) {
    
    if (argc == 2) {
        model = new Model(argv[1]);
    } else {
        model = new Model("data/african_head.obj");
    }
    
    TGAImage image(IMAGE_WIDTH, IMAGE_HEIGHT, TGAImage::RGB);
    
    std::cout << "Faces: " << model->nfaces();
    
    int nverts = model->nverts();
    
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        if (face.empty()) {
            continue;
        }
        
        for (int k = 0; k < APEX_COUNT; k++) {
            int vi0 = face[k];
            int vi1 = face[(k + 1) % APEX_COUNT];
            
            if (nverts < vi0 || nverts < vi1) {
                continue;
            }
            
            Vec3i v0 = transpose(model->vert(vi0));
            Vec3i v1 = transpose(model->vert(vi1));
            
            if (out_of_bounds(v0)) {
                std::cout << "out: " << v0.x << ":" << v0.y << "\n";
                continue;
            }
            
            if (out_of_bounds(v1)) {
                std::cout << "out: " << v1.x << ":" << v1.y << "\n";
                continue;
            }
            
            line(v0.x, v0.y, v1.x, v1.y, image, green);
        }
    }

    image.flip_vertically();
    image.write_tga_file("demo.tga");
    
    delete model;
    
    return 0;
}
