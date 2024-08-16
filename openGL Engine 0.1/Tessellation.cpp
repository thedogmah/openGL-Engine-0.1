#include "Tessellation.h"

Tessellation::Tessellation()
{

    VAO = 0;
    VBO = 0;
    EBO = 0;
    numVertices = 0;
    numIndices = 0;

}

Tessellation::~Tessellation()
{
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Tessellation::init()
{
    setupShaders();
    setupQuad();
//    setupTriangle();
 
}

void Tessellation::draw()
{
    //camera.getViewMatrix();
  
    glBindVertexArray(VAO);
    glUseProgram(TessShaderProgram);


    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "tess error befoer drawing: " << error << std::endl;
    }
    // Draw the quad


    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, 4);
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "tess error aftre drawing: " << error << std::endl;
    }
    // Unbind the VAO (optional)
    glBindVertexArray(0);
    glUseProgram(0);


}

void Tessellation::setMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix)
{
    glUseProgram(TessShaderProgram);
    GLuint viewLocation = glGetUniformLocation(TessShaderProgram, "view");
    GLuint projectionLocation = glGetUniformLocation(TessShaderProgram, "projection");

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(pViewMatrix));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(pProjectionMatrix));

}

bool Tessellation::checkShaderCompilation(GLint shader)
{

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
    
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> errorLog(logLength);
        glGetShaderInfoLog(shader, logLength, NULL, errorLog.data());
        std::cerr << "Shader compilation error:\n" << errorLog.data() << std::endl;
        // You may choose to handle the error in a different way (e.g., throw an exception)
        return false;
    }
    return true;
}

void Tessellation::setupQuad()
{

    float vertices[] = {
        // Positions          
         1.0f,  1.0f, 0.0f,   // Top right
         1.0f, -1.0f, 0.0f,   // Bottom right
        -1.0f, -1.0f, 0.0f,   // Bottom left
        -1.0f,  1.0f, 0.0f    // Top left 
    };


    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    // Unbind VAO
    glBindVertexArray(0);

}

void Tessellation::setupShaders()
{

    //// Vertex shader source code
    //const char* vertexShaderSource = "#version 330 core\n"
    //    "layout (location = 0) in vec3 aPos;\n"
    //    "void main()\n"
    //    "{\n"
    //    "   gl_Position = vec4(aPos, 1.0);\n"
    //    "}\0";

    // int gl_PatchVerticesIn

    //This built - in variable specifies the number of vertices in the input patch.
    //    It indicates the size of the input patch that the tessellation control shader operates on.
    //    The value of gl_PatchVerticesIn is determined by the layout declaration specified in the tessellation control shader, such as layout(vertices = N) out; .
    //    // int gl_PrimitiveID

    //    This built - in variable provides the index of the current patch being processed by the tessellation control shader.
    //    Each patch processed by the tessellation control shader is assigned a unique primitive ID.
    //    The gl_PrimitiveID variable is useful for performing per - patch operations or accessing data associated with specific patches.
    //    // int gl_InvocationID

    //    This built - in variable provides the index of the current vertex within the current patch being processed by the tessellation control shader.
    //    It ranges from 0 to the number of vertices in the patch minus one.
    //    The gl_InvocationID variable allows for performing per - vertex operations or accessing data associated with specific vertices within a patch.


   

    const char* vertexShaderSource = "#version 400 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"


        "void main()\n"
        "{\n"
        "    gl_Position = projection * view * vec4(aPos, 1.0);\n"
        "}\0";

    //CP is control points/
    //TL is tessleation levels
    
      const char* tessControlShaderSource = R"(
    #version 400 core
  layout(vertices = 4) out;

void main()
{
    // Pass through the input positions to the output
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    
    // Set tessellation levels (if applicable)
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = 4.0; // Set inner tessellation level
        gl_TessLevelInner[1] = 4.0; // Set inner tessellation level
        gl_TessLevelOuter[0] = 4.0; // Set outer tessellation level
        gl_TessLevelOuter[1] = 4.0; // Set outer tessellation level
        gl_TessLevelOuter[2] = 4.0; // Set outer tessellation level
        gl_TessLevelOuter[3] = 4.0; // Set outer tessellation level
    }
}
)";


      const char* tessEvalShaderSource = "#version 400 core\n"
          "layout(quads, equal_spacing) in;\n"
          "void main()\n"
          "{\n"
          "   vec3 p0 = mix(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_TessCoord.x);\n"
          "   vec3 p1 = mix(gl_in[2].gl_Position.xyz, gl_in[3].gl_Position.xyz, gl_TessCoord.x);\n"
          "   vec3 finalPos = mix(p0, p1, gl_TessCoord.y);\n"
          "   gl_Position = vec4(finalPos, 1.0);\n"
          "}\0";


        const char* fragmentShaderSource = "#version 400 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0, 0.5, 0.2, 0.5);\n"
        "}\0";

        //Compile Vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        checkShaderCompilation(vertexShader);
        //Compile tessellation control shader
        std::cout << "\nVertex Shader Fin\n";
        GLuint tessControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tessControlShader, 1, &tessControlShaderSource, NULL);
        glCompileShader(tessControlShader);
        checkShaderCompilation(tessControlShader);
        std::cout << "\nVertex Control Shader Fin\n";
        // Compile tessellation evaluation shader
        GLuint tessEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tessEvalShader, 1, &tessEvalShaderSource, NULL);
        glCompileShader(tessEvalShader);
        std::cout << "\nVertex Eval Shader Fin\n";
        checkShaderCompilation(tessEvalShader);
        // Compile fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        checkShaderCompilation(fragmentShader);
        std::cout << "\ntess frag Shader Fin\n";
     
        //Link shaders into program
        TessShaderProgram = glCreateProgram();
        
        glAttachShader(TessShaderProgram, vertexShader);
        glAttachShader(TessShaderProgram, tessControlShader);
       glAttachShader(TessShaderProgram, tessEvalShader);
        glAttachShader(TessShaderProgram, fragmentShader);
        
        //Currently no error checking for attaching shaders

        glLinkProgram(TessShaderProgram);

        GLint linkStatus;
        glGetProgramiv(TessShaderProgram, GL_LINK_STATUS, &linkStatus);
        if (linkStatus == GL_FALSE) {
            GLint logLength;
            glGetProgramiv(TessShaderProgram, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> log(logLength);
            glGetProgramInfoLog(TessShaderProgram, logLength, NULL, log.data());
            std::cerr << "Program linking error:\n" << log.data() << std::endl;
            // Handle linking error appropriately
        }
        else
            std::cout << "Tessellation program linked correctly";
}