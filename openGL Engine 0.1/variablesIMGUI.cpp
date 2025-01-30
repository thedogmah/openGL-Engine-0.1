// ImGuiVariables.cpp
#include "ImGuiVariables.h"
//#include "SSBO.h"
int selectedMeshIndex = 0;
int intSelectedmapSSBOMeshInstance = 0;

int selectedSSBOID = 0; //variable to remember what item in the SSBO edit imgui is selected
const SSBO* tempSelectedSSBO = nullptr;
SSBO* selectedSSBO = nullptr;
Mesh* selectedMeshInstance = nullptr;
//int intSelectedmapSSBOMeshInstance = 0;//variable to remember what item in the SSBO edit imgui is selected
bool showSecondDropdown = false;
//const char* filename ="";
std::string fname = "";
std::string fileNameBuffer2 = "File name";
std::string meshNameBuffer = "Mesh name";
std::string meshNameBufferInstanced = "Mesh instance name";
std::string stringMeshPathOverride = "";
bool boolPathOverride = false;
bool boolSSBOCombo3isSelected = false;
glm::mat4 globalModelMatrix = glm::mat4(1.0f);