#include "lince/renderer/transform.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>


void test_point(void** state) {
    (void)state;

    float screen_w = 350, screen_h = 500;
    LinceCamera cam;
    LinceInitCamera(&cam, screen_w/screen_h);

    LincePoint pix = { .x = 50, .y = 100 };
    LincePoint scr = LincePointPixelToScreen(pix, screen_w, screen_h);
    LincePoint wld = LincePointScreenToWorld(scr, &cam);

    LincePoint scr2 = LincePointWorldToScreen(wld, &cam);
    assert_float_equal(scr.x, scr2.x, 1e-5);

    LincePoint pix2 = LincePointScreenToPixel(scr2, screen_w, screen_h);
    assert_float_equal(pix.x, pix2.x, 1e-5);
}

void assert_transform_equal(LinceTransform* a, LinceTransform* b) {
    assert_float_equal(a->x, b->x, 1e-5);
    assert_float_equal(a->y, b->y, 1e-5);
    assert_float_equal(a->w, b->w, 1e-5);
    assert_float_equal(a->h, b->h, 1e-5);
    assert_int_equal(a->coords, b->coords);
}

void test_transform(void** state) {
    (void)state;

    float screen_w = 350, screen_h = 500;
    LinceCamera cam;
    LinceInitCamera(&cam, screen_w / screen_h);

    LinceTransform wld = { .x = 1.0, .y = 2.0, .w = 0.5, .h = 0.25 };
    assert_int_equal(wld.coords, LinceCoordSystem_World);

    // World -> World
    LinceTransform wld2;
    LinceTransformToWorld(&wld2, &wld, &cam, screen_w, screen_h);
    assert_transform_equal(&wld, &wld2);

    // World -> Screen
    LinceTransform scr;
    LinceTransformToScreen(&scr, &wld, &cam, screen_w, screen_h);
    assert_int_equal(scr.coords, LinceCoordSystem_Screen);

    // Screen -> Pixel, and World -> Pixel
    LinceTransform pix1, pix2;
    LinceTransformToPixel(&pix1, &wld, &cam, screen_w, screen_h);
    LinceTransformToPixel(&pix2, &scr, &cam, screen_w, screen_h);
    assert_int_equal(pix1.coords, LinceCoordSystem_Pixel);
    assert_int_equal(pix2.coords, LinceCoordSystem_Pixel);
    assert_transform_equal(&pix1, &pix2);

    // Pixel -> Screen
    LinceTransform scr2;
    LinceTransformToScreen(&scr2, &pix1, &cam, screen_w, screen_h);
    assert_int_equal(scr2.coords, LinceCoordSystem_Screen);
    assert_transform_equal(&scr, &scr2);

    // Screen -> World, and Pixel -> World
    LinceTransform wld3, wld4;
    LinceTransformToWorld(&wld3, &scr2, &cam, screen_w, screen_h);
    LinceTransformToWorld(&wld4, &pix1, &cam, screen_w, screen_h);
    assert_int_equal(wld3.coords, LinceCoordSystem_World);
    assert_transform_equal(&wld3, &wld4);
}