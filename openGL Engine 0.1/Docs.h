#pragma once
/*

RawModel Class can take a VAO ID uing the loader class 
and create a 3d mesh

the mesh can then get vertice count for GL draw functions
we can also get the VAOid from the raw mesh nae like 'quad->getVaoID'
this is used a lot in the quad for the water tiles.
RawModel Class uses Loaded class which 'loadsToVAO' and
returns that VAO ID to rawmodel
*/