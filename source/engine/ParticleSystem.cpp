#include "ginkgopch.h"
#include "ParticleSystem.h"
#include <cstdlib>

ParticleSystemO::ParticleSystemO() :ParticleSystemProps(), UpdateShader(), RenderShader(), buffer(), pointBuffer1(num_particles, Particle()), vaoID()
{
	demo_init();
}

ParticleSystemO::ParticleSystemO(const ParticleSystemProps& in):ParticleSystemProps(in), UpdateShader(), RenderShader(), buffer(), pointBuffer1(num_particles, Particle()), vaoID()
{
	demo_init();
}

void ParticleSystemO::demo_init()
{
	glGenBuffers(1, &buffer[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, pointBuffer1.size() * sizeof(Particle), &pointBuffer1[0], GL_STREAM_DRAW);
	glGenBuffers(1, &buffer[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, pointBuffer1.size() * sizeof(Particle), &pointBuffer1[0], GL_STREAM_DRAW);


	std::srand(0);
	for (auto i = 0; i < 2 * 256 * 256; ++i)
	{
		RGIMG[i] = std::rand() % 255;
	}
	glGenTextures(1, &texImg);
	glBindTexture(GL_TEXTURE_2D, texImg);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, 256, 256, 0, GL_RG, GL_UNSIGNED_BYTE, &RGIMG);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const GLchar* tmpar[] = {"v_Pos", "v_Age", "v_Life", "v_Vel"};
	UpdateShader.AddShader("shaders/psystemupdate.vert", GL_VERTEX_SHADER);
	UpdateShader.AddShader("shaders/psystemupdate.frag", GL_FRAGMENT_SHADER);
	glTransformFeedbackVaryings(UpdateShader.programId, 4, tmpar, GL_INTERLEAVED_ATTRIBS);
	//glBindAttribLocation(UpdateShader.programId, 0, "i_Pos");
	//glBindAttribLocation(UpdateShader.programId, 1, "i_Age");
	//glBindAttribLocation(UpdateShader.programId, 2, "i_Life");
	//glBindAttribLocation(UpdateShader.programId, 3, "i_Vel");	

	UpdateShader.LinkProgram();

	glGenVertexArrays(1, &vaoID[0]);
	glBindVertexArray(vaoID[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 2, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 2, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, vel));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vaoID[1]);
	glBindVertexArray(vaoID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 2, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 2, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	RenderShader.AddShader("shaders/particleRender.vert", GL_VERTEX_SHADER);
	RenderShader.AddShader("shaders/particleRender.frag", GL_FRAGMENT_SHADER);

	/*glBindAttribLocation(RenderShader.programId, 0, "i_Pos");*/
	RenderShader.LinkProgram();
	/*glBindAttribLocation(RenderShader.programId, 1, "i_Age");
	glBindAttribLocation(RenderShader.programId, 2, "i_Life");
	glBindAttribLocation(RenderShader.programId, 3, "i_Vel");*/

	
	

	glGenVertexArrays(1, &vaoID[2]);
	glBindVertexArray(vaoID[2]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(RenderShader.programId, "i_Pos"), 2, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(RenderShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(RenderShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(RenderShader.programId, "i_Vel"), 2, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vaoID[3]);
	glBindVertexArray(vaoID[3]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(RenderShader.programId, "i_Pos"), 2, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(RenderShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(RenderShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(RenderShader.programId, "i_Vel"), 2, GL_FLOAT, GL_FALSE, 4 * 6, (GLvoid*)offsetof(Particle, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

}

void ParticleSystemO::render(float dt)
{
	auto numparticles = live_part;

	/* Calculate time delta. */

	/* Here's where birth rate parameter comes into play.
	   We add to the number of active particles in the system
	   based on birth rate and elapsed time. */
	
	if (live_part < num_particles)
	{
		live_part = std::min(num_particles, (unsigned)(live_part + freq_birth * dt));
	}

	UpdateShader.UseShader();
	glUseProgram(UpdateShader.programId);
	glUniform1f(glGetUniformLocation(UpdateShader.programId,"u_DelTime"),dt);
	glUniform2f(glGetUniformLocation(UpdateShader.programId, "u_Force"), force[0], force[1]);
	glUniform2f(glGetUniformLocation(UpdateShader.programId, "u_StartPos"), pos[0],pos[1]);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinTheta"), min_theta);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxTheta"), max_theta);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinSpeed"), min_speed);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxSpeed"), max_speed);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinLife"), min_age);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxLife"), max_age);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texImg);
	glUniform1i(glGetUniformLocation(UpdateShader.programId, "u_RGNoise"), 0);


	glBindVertexArray(vaoID[state_read]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer[state_write]);
	glEnable(GL_RASTERIZER_DISCARD);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS,0, numparticles);
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,0);

	glBindVertexArray(vaoID[state_read + 2]);
	RenderShader.UseShader();	
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_POINTS, 0, numparticles);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDepthFunc(GL_LESS);
	std::swap(state_read, state_write);
}

ParticleSystem::ParticleSystem()
{

}

void ParticleSystem::demo_init()
{
	init_buffers();
	init_UpdateShader();
	init_RenderShader();
}

void ParticleSystem::render(float dt)
{
	auto numparticles = live_part;

	/* Calculate time delta. */

	/* Here's where birth rate parameter comes into play.
	   We add to the number of active particles in the system
	   based on birth rate and elapsed time. */

	/*if (live_part < num_particles)
	{
		live_part = std::min(num_particles, (unsigned)(live_part + freq_birth * dt));
	}*/

	use_UpdateShader();

	glBindVertexArray(vaoID[state_read]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer[state_write]);
	glEnable(GL_RASTERIZER_DISCARD);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, numparticles);
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	glBindVertexArray(vaoID[state_read + 2]);

	use_RenderShader();

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_POINTS, 0, numparticles);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDepthFunc(GL_LESS);
	std::swap(state_read, state_write);
}

ParticleSystem3DSimpleBehavior::ParticleSystem3DSimpleBehavior():ParticleSystemType<Particle3D, ParticleSystemProps3D>(),texImg()
{
	//init the image buffer that we want
	std::srand(0);
	for (auto i = 0; i < 3 * 512 * 512; ++i)
	{
		RGBIMG[i] = std::rand() % 255;
	}
}

ParticleSystem3DSimpleBehavior::ParticleSystem3DSimpleBehavior(const ParticleSystemProps3D& in) :ParticleSystemType<Particle3D, ParticleSystemProps3D>(in), texImg()
{
	std::srand(0);
	for (auto i = 0; i < 3 * 512 * 512; ++i)
	{
		RGBIMG[i] = std::rand() % 255;
	}
}

void ParticleSystem3DSimpleBehavior::init_buffers()
{
	glGenTextures(1, &texImg);
	glBindTexture(GL_TEXTURE_2D, texImg);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, &RGBIMG);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem3DSimpleBehavior::init_UpdateShader()
{
	const GLchar* tmpar[] = { "v_Pos", "v_Age", "v_Life", "v_Vel" };
	UpdateShader.AddShader("shaders/psystemupdate3Ddefault.vert", GL_VERTEX_SHADER);
	UpdateShader.AddShader("shaders/psystemupdate3Ddefault.frag", GL_FRAGMENT_SHADER);
	glTransformFeedbackVaryings(UpdateShader.programId, 4, tmpar, GL_INTERLEAVED_ATTRIBS);
	//glBindAttribLocation(UpdateShader.programId, 0, "i_Pos");
	//glBindAttribLocation(UpdateShader.programId, 1, "i_Age");
	//glBindAttribLocation(UpdateShader.programId, 2, "i_Life");
	//glBindAttribLocation(UpdateShader.programId, 3, "i_Vel");	

	UpdateShader.LinkProgram();

	glGenVertexArrays(1, &vaoID[0]);
	glBindVertexArray(vaoID[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vaoID[1]);
	glBindVertexArray(vaoID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem3DSimpleBehavior::init_RenderShader()
{
	RenderShader.AddShader("shaders/particleRenderBasic3D.vert", GL_VERTEX_SHADER);
	RenderShader.AddShader("shaders/particleRenderBasic3D.frag", GL_FRAGMENT_SHADER);

	/*glBindAttribLocation(RenderShader.programId, 0, "i_Pos");*/
	RenderShader.LinkProgram();
	/*glBindAttribLocation(RenderShader.programId, 1, "i_Age");
	glBindAttribLocation(RenderShader.programId, 2, "i_Life");
	glBindAttribLocation(RenderShader.programId, 3, "i_Vel");*/




	glGenVertexArrays(1, &vaoID[2]);
	glBindVertexArray(vaoID[2]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vaoID[3]);
	glBindVertexArray(vaoID[3]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem3DSimpleBehavior::use_UpdateShader(float dt)
{
	UpdateShader.UseShader();
	//glUseProgram(UpdateShader.programId);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_DelTime"), dt);
	glUniform3f(glGetUniformLocation(UpdateShader.programId, "u_Force"), force[0], force[1],force[2]);
	glUniform3f(glGetUniformLocation(UpdateShader.programId, "u_StartPos"), pos[0], pos[1],pos[2]);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinTheta"), min_theta);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxTheta"), max_theta);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinSpeed"), min_speed);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxSpeed"), max_speed);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinLife"), min_age);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxLife"), max_age);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texImg);
	glUniform1i(glGetUniformLocation(UpdateShader.programId, "u_RGNoise"), 0);
}

void ParticleSystem3DSimpleBehavior::use_RenderShader(float dt)
{
	RenderShader.UseShader();
	glUniformMatrix4fv(glGetUniformLocation(RenderShader.programId, "u_projection"), 1, GL_FALSE, projection);
	glUniformMatrix4fv(glGetUniformLocation(RenderShader.programId, "u_view"), 1, GL_FALSE, view);
}



ParticleSystem1::ParticleSystem1() :ParticleSystemProps3D(), UpdateShader(), RenderShader(), buffer(), pointBuffer(num_particles, Particle3D()), vaoID()
{
	std::srand(0);
	for (auto i = 0; i < 3 * 256 * 256; ++i)
	{
		RGBIMG[i] = std::rand() % 255;
	}
	internal_init();
}

ParticleSystem1::ParticleSystem1(const ParticleSystemProps3D& in) :ParticleSystemProps3D(in), UpdateShader(), RenderShader(), buffer(), pointBuffer(num_particles, Particle3D()), vaoID()
{
	std::srand(0);
	for (auto i = 0; i < 3 * 256 * 256; ++i)
	{
		RGBIMG[i] = std::rand() % 255;
	}
	internal_init();
}

void ParticleSystem1::internal_init()
{
	glGenBuffers(1, &buffer[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, pointBuffer.size() * sizeof(Particle3D), &pointBuffer[0], GL_STREAM_DRAW);
	glGenBuffers(1, &buffer[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, pointBuffer.size() * sizeof(Particle3D), &pointBuffer[0], GL_STREAM_DRAW);
	init_buffers();
	init_UpdateShader();
	init_RenderShader();
}

void ParticleSystem1::init_buffers()
{
	glGenTextures(1, &texImg);
	glBindTexture(GL_TEXTURE_2D, texImg);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, &RGBIMG);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem1::init_UpdateShader()
{
	const GLchar* tmpar[] = { "v_Pos", "v_Age", "v_Life", "v_Vel" };
	UpdateShader.AddShader("shaders/psystemupdate3Ddefault.vert", GL_VERTEX_SHADER);
	UpdateShader.AddShader("shaders/psystemupdate3Ddefault.frag", GL_FRAGMENT_SHADER);
	glTransformFeedbackVaryings(UpdateShader.programId, 4, tmpar, GL_INTERLEAVED_ATTRIBS);
	//glBindAttribLocation(UpdateShader.programId, 0, "i_Pos");
	//glBindAttribLocation(UpdateShader.programId, 1, "i_Age");
	//glBindAttribLocation(UpdateShader.programId, 2, "i_Life");
	//glBindAttribLocation(UpdateShader.programId, 3, "i_Vel");	

	UpdateShader.LinkProgram();

	glGenVertexArrays(1, &vaoID[0]);
	glBindVertexArray(vaoID[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vaoID[1]);
	glBindVertexArray(vaoID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem1::init_RenderShader()
{
	RenderShader.AddShader("shaders/particleRenderBasic3D.vert", GL_VERTEX_SHADER);
	RenderShader.AddShader("shaders/particleRenderBasic3D.frag", GL_FRAGMENT_SHADER);

	/*glBindAttribLocation(RenderShader.programId, 0, "i_Pos");*/
	RenderShader.LinkProgram();
	/*glBindAttribLocation(RenderShader.programId, 1, "i_Age");
	glBindAttribLocation(RenderShader.programId, 2, "i_Life");
	glBindAttribLocation(RenderShader.programId, 3, "i_Vel");*/




	glGenVertexArrays(1, &vaoID[2]);
	glBindVertexArray(vaoID[2]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vaoID[3]);
	glBindVertexArray(vaoID[3]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem1::use_UpdateShader(float dt)
{
	UpdateShader.UseShader();
	//glUseProgram(UpdateShader.programId);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_DelTime"), dt);
	glUniform3f(glGetUniformLocation(UpdateShader.programId, "u_Force"), force[0], force[1],force[2]);
	glUniform3f(glGetUniformLocation(UpdateShader.programId, "u_StartPos"), pos[0], pos[1],pos[2]);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinTheta"), min_theta);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxTheta"), max_theta);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinSpeed"), min_speed);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxSpeed"), max_speed);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinLife"), min_age);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxLife"), max_age);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texImg);
	glUniform1i(glGetUniformLocation(UpdateShader.programId, "u_RGNoise"), 0);
}

void ParticleSystem1::use_RenderShader(float dt)
{
	RenderShader.UseShader();
	glUniformMatrix4fv(glGetUniformLocation(RenderShader.programId, "u_projection"), 1, GL_FALSE, projection);
	glUniformMatrix4fv(glGetUniformLocation(RenderShader.programId, "u_view"), 1, GL_FALSE, view);
}

void ParticleSystem1::render(float dt)
{
	unsigned numparticles = live_part;
	live_part_f += this->freq_birth * dt;
	if (live_part < this->num_particles)
	{
		live_part = std::min(this->num_particles, (unsigned)(live_part_f));
		//this emission pattern doesnt work.
	}
	else
	{
		//std::cout << live_part << std::endl;
	}
	use_UpdateShader(dt);
	glBindVertexArray(vaoID[state_read]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer[state_write]);
	glEnable(GL_RASTERIZER_DISCARD);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, numparticles);
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	glBindVertexArray(vaoID[state_read + 2]);

	use_RenderShader(dt);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_POINTS, 0, numparticles);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LESS);
	std::swap(state_read, state_write);
}


ParticleSystem3DVectorFields::ParticleSystem3DVectorFields():ParticleSystemProps3D(), pointBuffer(num_particles, Particle3D()),texImg(), texImg1(),scale(5,5,5)
{
	//init the image buffer that we want
	int k = 256 * 256 * 2;
	for (auto i = 0; i < 256; ++i)
	{
		unsigned char ii = 255 - i;
		for (auto j = 0; j < 256; ++j)
		{
			unsigned char jj = 255 - j;
				int ind = (i * 256 + j) * 2;

				if (ind >= k)
				{
					continue;
				}
				RGBIMG[ind] = i;
				RGBIMG[ind + 1] = j;

				RGBIMG1[ind] = j;
				RGBIMG1[ind + 1] = ii;
			
		}
	}

	internal_init();
}

ParticleSystem3DVectorFields::ParticleSystem3DVectorFields(const ParticleSystemProps3D& in) :ParticleSystemProps3D(in), pointBuffer(num_particles, Particle3D()),texImg(), texImg1(), scale(5, 5, 5)
{
	//init the image buffer that we want
	for (auto i = 0; i < 256; ++i)
	{
		unsigned char ii = 255 - i;
		for (auto j = 0; j < 256; ++j)
		{
			unsigned char jj = 255 - j;
			auto ind = (i * 256 + j) * 2;
			RGBIMG[ind] = jj;
			RGBIMG[ind + 1] = i;

			RGBIMG1[ind] = 0;
			RGBIMG1[ind + 1] = ii;

		}
	}
	internal_init();
}

void ParticleSystem3DVectorFields::internal_init()
{

	glGenBuffers(1, &buffer[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, pointBuffer.size() * sizeof(Particle3D), &pointBuffer[0], GL_STREAM_DRAW);

	glGenBuffers(1, &buffer[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, pointBuffer.size() * sizeof(Particle3D), &pointBuffer[0], GL_STREAM_DRAW);
	init_buffers();
	init_UpdateShader();
	init_RenderShader();
}

void ParticleSystem3DVectorFields::render(float dt)
{
	unsigned numparticles = live_part;
	live_part_f += this->freq_birth * dt;
	if (live_part < this->num_particles)
	{
		live_part = std::min(this->num_particles, (unsigned)(live_part_f));
		//this emission pattern doesnt work.
	}
	else
	{
		//std::cout << live_part << std::endl;
	}
	use_UpdateShader(dt);
	glBindVertexArray(vaoID[state_read]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer[state_write]);
	glEnable(GL_RASTERIZER_DISCARD);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, numparticles);
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	glBindVertexArray(vaoID[state_read + 2]);

	use_RenderShader(dt);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_POINTS, 0, numparticles);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LESS);
	std::swap(state_read, state_write);
}

void ParticleSystem3DVectorFields::init_buffers()
{
	glGenTextures(1, &texImg);
	glBindTexture(GL_TEXTURE_2D, texImg);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, 256, 256, 0, GL_RG, GL_UNSIGNED_BYTE, &RGBIMG);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glGenTextures(1, &texImg1);
	glBindTexture(GL_TEXTURE_2D, texImg1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, 256, 256, 0, GL_RG, GL_UNSIGNED_BYTE, &RGBIMG1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem3DVectorFields::init_UpdateShader()
{
	const GLchar* tmpar[] = { "v_Pos", "v_Age", "v_Life", "v_Vel" };
	UpdateShader.AddShader("shaders/psystemupdate3DdefaultFlowField.vert", GL_VERTEX_SHADER);
	UpdateShader.AddShader("shaders/psystemupdate3Ddefault.frag", GL_FRAGMENT_SHADER);
	glTransformFeedbackVaryings(UpdateShader.programId, 4, tmpar, GL_INTERLEAVED_ATTRIBS);
	//glBindAttribLocation(UpdateShader.programId, 0, "i_Pos");
	//glBindAttribLocation(UpdateShader.programId, 1, "i_Age");
	//glBindAttribLocation(UpdateShader.programId, 2, "i_Life");
	//glBindAttribLocation(UpdateShader.programId, 3, "i_Vel");	

	UpdateShader.LinkProgram();

	glGenVertexArrays(1, &vaoID[0]);
	glBindVertexArray(vaoID[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vaoID[1]);
	glBindVertexArray(vaoID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem3DVectorFields::init_RenderShader()
{
	RenderShader.AddShader("shaders/particleRenderBasic3D.vert", GL_VERTEX_SHADER);
	RenderShader.AddShader("shaders/particleRenderBasic3D.frag", GL_FRAGMENT_SHADER);

	/*glBindAttribLocation(RenderShader.programId, 0, "i_Pos");*/
	RenderShader.LinkProgram();
	/*glBindAttribLocation(RenderShader.programId, 1, "i_Age");
	glBindAttribLocation(RenderShader.programId, 2, "i_Life");
	glBindAttribLocation(RenderShader.programId, 3, "i_Vel");*/




	glGenVertexArrays(1, &vaoID[2]);
	glBindVertexArray(vaoID[2]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vaoID[3]);
	glBindVertexArray(vaoID[3]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Age"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, age));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Life"), 1, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, life));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(glGetAttribLocation(UpdateShader.programId, "i_Vel"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle3D), (GLvoid*)offsetof(Particle3D, vel));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem3DVectorFields::use_UpdateShader(float dt)
{
	UpdateShader.UseShader();
	//glUseProgram(UpdateShader.programId);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_DelTime"), dt);
	glUniform3f(glGetUniformLocation(UpdateShader.programId, "u_Force"), force[0], force[1], force[2]);
	glUniform3f(glGetUniformLocation(UpdateShader.programId, "u_StartPos"), pos[0], pos[1], pos[2]);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinTheta"), min_theta);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxTheta"), max_theta);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinSpeed"), min_speed);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxSpeed"), max_speed);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MinLife"), min_age);
	glUniform1f(glGetUniformLocation(UpdateShader.programId, "u_MaxLife"), max_age);
	glUniform3f(glGetUniformLocation(UpdateShader.programId, "u_Scale"), scale[0], scale[1], scale[2]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texImg);
	glUniform1i(glGetUniformLocation(UpdateShader.programId, "u_RGNoise"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texImg1);
	glUniform1i(glGetUniformLocation(UpdateShader.programId, "u_RGNoise1"), 0);
}

void ParticleSystem3DVectorFields::use_RenderShader(float dt)
{
	RenderShader.UseShader();
	glUniformMatrix4fv(glGetUniformLocation(RenderShader.programId, "u_projection"), 1, GL_FALSE, projection);
	glUniformMatrix4fv(glGetUniformLocation(RenderShader.programId, "u_view"), 1, GL_FALSE, view);
}