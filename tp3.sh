#!/bin/bash
donnees=`ls p1000_5/`

for x in $donnees
do
	./tp3 p1000_5/$x
done


