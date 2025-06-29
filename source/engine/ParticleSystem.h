#pragma once
//#include "ginkgopch.h"

#include "shader.h"
#include <array>
#include <concepts>
#include <type_traits>
#include <algorithm>
#define MAX_PARTS_POOL 10000


template<class T>
concept IsParticle = requires(T a)
{
	a.pos;
};
template<class T>
concept IsParticleSystemProps = requires(T a)
{
	{ a.num_particles } -> std::convertible_to<unsigned>;
	{ a.freq_birth } -> std::convertible_to<float>;
	a.num_particles;
	a.freq_birth;
};


template<typename T>
struct ParticleGeneric
{
	T pos = T();
	float age = 0.0f;
	float life = 0.0f;
	T vel = T();
};



typedef ParticleGeneric<glm::vec2> Particle2D;
typedef ParticleGeneric<glm::vec3> Particle3D;
//
struct Particle
{
	glm::vec2 pos = glm::vec2(0.0f);
	float age = 0.0f;
	float life = 0.0f;
	glm::vec2 vel = glm::vec2(0.0f);
};
template<typename T>
struct ParticleSystemPropsGeneric
{
	unsigned num_particles = 10000;
	float freq_birth = 10.0f;
	float min_age = 1.0f, max_age = 2.05f;
	float min_theta = -1.0f; float max_theta = 1.0f;
	float min_speed = 0.5f; float max_speed = 4.0f;
	T pos = T();
	T force = T();
};
typedef ParticleSystemPropsGeneric<glm::vec2> ParticleSystemProps2D;
typedef ParticleSystemPropsGeneric<glm::vec3> ParticleSystemProps3D;
struct ParticleSystemProps
{
	unsigned num_particles = 10000;
	float freq_birth = 1000.0f;
	float min_age=0.5f, max_age=3.0f;
	float min_theta = -1.0f; float max_theta = 1.0f;
	float min_speed = 1.05f; float max_speed = 10.01f;
	glm::vec2 pos = glm::vec2(0.0f, 0.0f);
	glm::vec2 force = glm::vec2(0.0f, -1.1f);
};

template <typename T>
class ParticleSystemGeneric
{
	public:
		Shader UpdateShader;
		Shader RenderShader;
		std::array<GLuint, 2>buffer;
		std::array<GLuint, 4>vaoID;
		std::vector<ParticleGeneric<T>> pointBuffer;
private:
		unsigned live_part = 0;
		unsigned state_read = 0;
		unsigned state_write = 1;
};



class ParticleSystem
{
	public:
		ParticleSystem();
		void demo_init();
		void render(float dt);
		unsigned live_part = 0;
		unsigned state_read = 0;
		unsigned state_write = 1;
		Shader UpdateShader;
		Shader RenderShader;
		std::array<GLuint, 2>buffer;
		std::array<GLuint, 4>vaoID;
	//pure virtual implementation
		virtual void init_buffers() = 0;
		virtual void init_UpdateShader() = 0;
		virtual void init_RenderShader() = 0;
		virtual void use_UpdateShader() = 0;
		virtual void use_RenderShader() = 0;
		//virtual void buffer_init_binding_UpdateShader() = 0;
};

template <IsParticle T, IsParticleSystemProps Y>
class ParticleSystemType:public Y
{	
	public:
		ParticleSystemType():Y(), live_part(0), state_read(0), state_write(1), UpdateShader(), RenderShader()
		{
			buffer.fill(GLuint());
			vaoID.fill(GLuint());
			pointBuffer.fill(T());
			this->num_particles = unsigned(MAX_PARTS_POOL);
			//internal_init();
			
		};
		ParticleSystemType(const Y& why) :Y(why), live_part(0), state_read(0), state_write(1), UpdateShader(), RenderShader()
		{
			buffer.fill(GLuint());
			vaoID.fill(GLuint());
			pointBuffer.fill(T());
			this->num_particles =  unsigned(MAX_PARTS_POOL);
			//internal_init();
		};
		void internal_init();
		void render(float dt);
		unsigned live_part = 0;
		unsigned state_read = 0;
		unsigned state_write = 1;
		Shader UpdateShader;
		Shader RenderShader;
		std::array<GLuint, 2>buffer;
		std::array<GLuint, 4>vaoID;
		std::array<T, MAX_PARTS_POOL> pointBuffer;
		//pure virtual implementation
		virtual void init_buffers() = 0;
		virtual void init_UpdateShader() = 0;
		virtual void init_RenderShader() = 0;
		virtual void use_UpdateShader(float dt) = 0;
		virtual void use_RenderShader(float dt) = 0;

};



class ParticleSystem3DSimpleBehavior: public ParticleSystemType<Particle3D, ParticleSystemProps3D>
{
	public:
		std::array<unsigned char, 3 * 512 * 512>RGBIMG;
		GLuint texImg;
		GLfloat* projection=nullptr;
		GLfloat* view = nullptr;
		ParticleSystem3DSimpleBehavior();
		ParticleSystem3DSimpleBehavior(const ParticleSystemProps3D& in);
	
		void pass_info(GLfloat* proj, GLfloat* vi)
		{
			projection = proj;
			view = vi;
		}
		void init_buffers() override;
		void init_UpdateShader()override;
		void init_RenderShader()override;
		void use_UpdateShader(float dt) override;
		void use_RenderShader(float dt) override;
};

class ParticleSystem3DVectorFields :public ParticleSystemProps3D
{
	public:
		Shader UpdateShader;
		Shader RenderShader;
		std::array<GLuint, 2>buffer;
		std::array<GLuint, 4>vaoID;
		std::vector<Particle3D> pointBuffer;
		glm::vec3 scale;
		unsigned live_part = 0;
		float live_part_f = 0;
		unsigned state_read = 0;
		unsigned state_write = 1;
		std::array<unsigned char, 2 * 256 * 256>RGBIMG;
		std::array<unsigned char, 2 * 256 * 256>RGBIMG1;
		GLuint texImg;
		GLuint texImg1;
		GLfloat* projection=nullptr;
		GLfloat* view = nullptr;
		ParticleSystem3DVectorFields();
		ParticleSystem3DVectorFields(const ParticleSystemProps3D& in);
		
		void pass_info(GLfloat* proj, GLfloat* vi)
		{
			projection = proj;
			view = vi;
		}
		void internal_init();
		void render(float dt);
		void init_buffers() ;
		void init_UpdateShader();
		void init_RenderShader();
		void use_UpdateShader(float dt) ;
		void use_RenderShader(float dt) ;
};

//create emitter behaviors?

class ParticleSystemO:public ParticleSystemProps
{
	public:
		Shader UpdateShader;
		Shader RenderShader;
		std::array<GLuint, 2>buffer;
		std::array<GLuint, 4>vaoID;
		std::vector<Particle> pointBuffer1;
		GLuint texImg;
		unsigned live_part=0;
		unsigned state_read = 0;
		unsigned state_write = 1;
		std::array<unsigned char, 2*256 * 256>RGIMG;
		ParticleSystemO();
		ParticleSystemO(const ParticleSystemProps& in);
		void demo_init();
		void render(float dt);
};

class ParticleSystem1 :public ParticleSystemProps3D
{
public:
	Shader UpdateShader;
	Shader RenderShader;
	std::array<GLuint, 2>buffer;
	std::array<GLuint, 4>vaoID;
	std::vector<Particle3D> pointBuffer;
	GLuint texImg;
	unsigned live_part = 0;
	float live_part_f = 0;
	unsigned state_read = 0;
	unsigned state_write = 1;
	std::array<unsigned char, 3 * 256 * 256>RGBIMG;
	GLfloat* projection = nullptr;
	GLfloat* view = nullptr;
	ParticleSystem1();
	ParticleSystem1(const ParticleSystemProps3D& in);
	void internal_init();
	void render(float dt);
	void pass_info(GLfloat* proj, GLfloat* vi)
	{
		projection = proj;
		view = vi;
	}
	void init_buffers() ;
	void init_UpdateShader();
	void init_RenderShader();
	void use_UpdateShader(float dt) ;
	void use_RenderShader(float dt) ;
};

template<IsParticle T, IsParticleSystemProps Y>
inline void ParticleSystemType<T, Y>::internal_init()
{
	glGenBuffers(1, &buffer[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, pointBuffer.size() * sizeof(T), &pointBuffer[0], GL_STREAM_DRAW);
	glGenBuffers(1, &buffer[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, pointBuffer.size() * sizeof(T), &pointBuffer[0], GL_STREAM_DRAW);
	init_buffers();
	init_UpdateShader();
	init_RenderShader();
}

template<IsParticle T, IsParticleSystemProps Y>
inline void ParticleSystemType<T, Y>::render(float dt)
{
	unsigned numparticles = live_part;
	if (live_part < this->num_particles)
	{
		live_part = std::min<unsigned>(this->num_particles, (unsigned)(live_part + this->freq_birth * dt));
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
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_POINTS, 0, numparticles);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDepthFunc(GL_LESS);
	std::swap(state_read, state_write);
}

