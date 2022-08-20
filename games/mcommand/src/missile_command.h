#ifndef MISSILE_COMMAND_H
#define MISSILE_COMMAND_H

#include <lince.h>

void MCommandOnAttach(LinceLayer* layer);
void MCommandOnUpdate(LinceLayer* layer, float dt);
void MCommandOnDetach(LinceLayer* layer);
LinceLayer* MCommandLayerInit();

#endif /* MISSILE_COMMAND_H */