#include <cmath>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

struct Mass
{
	float m;
	glm::vec3 pos;
	glm::vec3 vel;
	glm::vec3 force;

	Mass(float m) : m(m), pos(), vel(), force() {}
	~Mass() = default;

	void applyForce(glm::vec3 force)
	{
		this->force += force;
	}

	void init()
	{
		force = glm::vec3();
	}

	void simulate(float dt)
	{
		vel += (force / m) * dt;
		pos += vel * dt;
	}
};

struct Spring
{
	std::shared_ptr<Mass> mass1, mass2;

	// A constant to represent the stiffness of the spring
	float springConstant;
	float springLength;
	float frictionConstant;

	Spring(
		std::shared_ptr<Mass> mass1, std::shared_ptr<Mass> mass2, 
		float springConstant, float springLength, float frictionConstant
	):
		mass1{mass1}, mass2{mass2},
		springConstant{springConstant},
		springLength{springLength},
		frictionConstant{frictionConstant}
	{}
	
	~Spring() = default;

	void solve()
	{
		// vector between the two masses
		glm::vec3 springVector = mass1->pos - mass2->pos;
		// distance between the two masses
		float r = glm::length(springVector);
		// force initially has a zero value
		glm::vec3 e, force{};

		// to avoid a division by zero check if r is zero
		if(r > 0.00001f) {
			e = springVector / r;
			//the spring force is added to the force
			force += e * (r - springLength) * (-springConstant);
			//the friction force is added to the force
			force += -e * (mass1->vel*e - mass2->vel*e) * frictionConstant;
			//with this addition we obtain the net force of the spring
		}
		mass1->applyForce(force);
		mass2->applyForce(-force);
	}
};

struct Simulation
{
	int numOfMasses;
	std::vector<std::shared_ptr<Mass>> masses;

	Simulation(int numOfMasses, float m):
		numOfMasses{numOfMasses}, 
		masses{std::vector<std::shared_ptr<Mass>>(numOfMasses)}
	{
		for(int i = 0; i < numOfMasses; ++i)
			masses[i] = std::make_shared<Mass>(m);
	}
	~Simulation() = default;

	Mass& getMass(int idx)
	{
		return *masses.at(idx);
	}

	void init()
	{
		for(int i = 0; i < numOfMasses; ++i)
			masses[i]->init();
	}

	virtual void solve() = 0;

	virtual void simulate(float dt)
	{
		for(int i = 0; i < numOfMasses; ++i)
			masses[i]->simulate(dt);
	}

	void operate(float dt)
	{
		init();
		solve();
		simulate(dt);
	}
};

struct ClothSimulation : Simulation
{
	std::vector<std::unique_ptr<Spring>> springs;
	int n;
	glm::vec3 gravitation;
	glm::vec3 fixPos[2];
	glm::vec3 fixVel[2];

	ClothSimulation(
		int numOfMasses,
		float m,
		float springConstant,
		float springLength,
		float springFrictionConstant,
		glm::vec3 gravitation
	):
		Simulation(numOfMasses, m),
		n(sqrt(numOfMasses)),
		gravitation{gravitation}, 
		fixPos{glm::vec3(), glm::vec3(n * springLength, 0, 0)}
	{
		for(int i = 0; i < numOfMasses; ++i) {
			masses[i]->pos = glm::vec3((i % 10) * springLength, 0, 0);
			masses[i]->vel = glm::vec3();
		}

		for(int i = 0; i < numOfMasses; ++i) {
			if(i % n != 0) {
				springs.push_back(std::make_unique<Spring>(
					masses[i], masses[i - 1], 
					springConstant, springLength, springFrictionConstant
				));
			}
			if(i >= n) {
				springs.push_back(std::make_unique<Spring>(
					masses[i], masses[i - n],
					springConstant, springLength, springFrictionConstant
				));
			}
		}
	}
	~ClothSimulation() = default;

	void solve() override
	{
		for(auto& spring : springs) {
			spring->solve();
		}
		for(int i = 0; i < numOfMasses; ++i) {
			masses[i]->applyForce(gravitation * masses[i]->m);
		}
	}

	void simulate(float dt) override
	{
		Simulation::simulate(dt);
		for(int i = 0; i < 2; ++i) {
			fixPos[i] += fixVel[i] * dt;
			masses[i * (n - 1)]->pos = fixPos[i];
			masses[i * (n - 1)]->vel = fixVel[i];
		}
	}

	int getIdx(int x, int y)
	{
		return x + y * n;
	}
};