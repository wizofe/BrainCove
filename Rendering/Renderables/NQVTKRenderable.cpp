#include "NQVTKRenderable.h"

NQVTKRenderable::NQVTKRenderable() : shader(0)
{
	color = NQVTK::Vector3(1.0,0.0,0.0);
	opacity = 1;
	shader = CreateShader();

	glEnable ( GL_LIGHTING );
	glEnable(GL_LIGHT0);   

	glLineWidth(4);

	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { -1, -1.0f, 3.0f, 1.0f };
	GLfloat spot_direction[] = { -1.0, -1.0, 0.0 };
	GLfloat spot_cutoff = 15.0f;

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &spot_cutoff);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
}

std::string NQVTKRenderable::GetSourceFromFile(const char* filePath)
{
	QFile f(filePath);
	if(!f.exists())
		throw std::exception("File not found!");
	f.open(QIODevice::ReadOnly);
	QTextStream fs(&f);
	std::string returner = fs.readAll().toStdString();
	f.close();
	return returner;
}

GLProgram* NQVTKRenderable::CreateShader()
{
	std::string vShader = GetSourceFromFile(":/text/shaders/Lighting.vs");
	std::string fShader = GetSourceFromFile(":/text/shaders/Lighting.fs");

	glewInit();
	GLProgram *shader = GLProgram::New();
    if (!shader) 
		throw std::exception("Could not create shader program object");

    bool ok = true;
    if (ok) 
		ok = shader->AddVertexShader(vShader);
    if (ok) 
		ok = shader->AddFragmentShader(fShader);
    if (ok) 
		ok = shader->Link();
    if (!ok) 
	{
            delete shader;
			throw std::exception("Could not add fragment shader");
	}
    return shader;
}