#!/bin/bash

rm rootController.o
rm rootController
rm rootSectors.o
rm rootStress.o
rm rootModules.o
rm rootEvents.o
rm rootScheduller.o
rm activityObject.o
make rootSectors.o
make rootStress.o
make rootModules.o
make rootEvents.o
make rootController.o
make rootScheduller.o
make activityObject.o
make rootController
