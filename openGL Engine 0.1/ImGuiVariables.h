#pragma once
#include <string>
#include "SSBO.h"

	
extern int intSelectedmapSSBOMeshInstance;
extern int selectedMeshIndex;
extern std::string fileNameBuffer2;
extern std::string meshNameBuffer;
extern std::string stringMeshPathOverride;
extern bool boolPathOverride;
extern std::string fname;
extern std::string meshNameBufferInstanced;
extern int meshCountInstanced;
extern float scalemin;
extern float scalemax;
static int bindingIndexCount;
//extern static const char* filterPatterns[2];
extern const char* filename[];

extern int selectedSSBOID ; ////variable to remember what item in the SSBO edit imgui is selected and its bools
extern Mesh* selectedMeshInstance;
extern bool showSecondDropdown;
extern SSBO* selectedSSBO;
extern const SSBO* tempSelectedSSBO;
extern bool boolSSBOCombo3isSelected;
extern glm::mat4 globalModelMatrix;