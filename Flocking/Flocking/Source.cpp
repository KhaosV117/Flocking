#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>

using namespace sf;
using namespace std;
class Vehicle {

	Vector2f location;
	Vector2f velocity;
	Vector2f acceleration;
	// Additional variable for size
	float r;
	float maxforce;
	float maxspeed;

	Vehicle(float x, float y) {
		acceleration = Vector2f(0, 0);
		velocity =  Vector2f(0, 0);
		location = Vector2f(x, y);
		r = 3.0;
		//[full] Arbitrary values for maxspeed and
		// force; try varying these!
		maxspeed = 4;
		maxforce = 0.1;
		//[end]
	}

	// Our standard “Euler integration” motion model
	void update() {
		velocity = velocity + acceleration;//velocity.add(acceleration);
		float velocity_magnitude;
		velocity_magnitude = sqrtf((velocity.x*velocity.x)+(velocity.y*velocity.y));
		if (velocity_magnitude >= maxspeed)
		{
			float ratio = maxspeed / velocity_magnitude;
			velocity.x *= ratio;
			velocity.y *= ratio;
		}
		//velocity.limit(maxspeed);
		location = location + velocity;
		//location.add(velocity);
		acceleration = Vector2f(0,0);
	}

	// Newton’s second law; we could divide by mass if we wanted.
	void applyForce(Vector2f force) {
		acceleration += force;
	}

	// Our seek steering force algorithm
	void seek(Vector2f target) {
		Vector2f desired = target - location;
		desired.normalize();
		desired.mult(maxspeed);
		Vector2f steer = Vector2f.sub(desired, velocity);
		steer.limit(maxforce);
		applyForce(steer);
	}
	void normalize()
	{
		float velocity_magnitude;
		velocity_magnitude = sqrtf((velocity.x*velocity.x) + (velocity.y*velocity.y));


	}

	void display() {
		// Vehicle is a triangle pointing in
		// the direction of velocity; since it is drawn
		// pointing up, we rotate it an additional 90 degrees.
		float theta = velocity.heading() + PI / 2;
		fill(175);
		stroke(0);
		pushMatrix();
		translate(location.x, location.y);
		rotate(theta);
		beginShape();
		vertex(0, -r * 2);
		vertex(-r, r * 2);
		vertex(r, r * 2);
		endShape(CLOSE);
		popMatrix();
	}

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:

	ParticleSystem(unsigned int count) :
		m_particles(count),
		m_vertices(sf::Points, count),
		m_lifetime(sf::seconds(3)),
		m_emitter(0, 0)
	{
	}

	void setTarget(sf::Vector2f position)
	{
		m_emitter = position;
	}

	void update(sf::Time elapsed)
	{
		for (std::size_t i = 0; i < m_particles.size(); ++i)
		{
			// update the particle lifetime
			Particle& p = m_particles[i];
			p.lifetime -= elapsed;

			// if the particle is dead, respawn it
			if (p.lifetime <= sf::Time::Zero)
				resetParticle(i);

			// update the position of the corresponding vertex
			m_vertices[i].position += p.velocity * elapsed.asSeconds();

			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
			m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
		}
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// our particles don't use a texture
		states.texture = NULL;

		// draw the vertex array
		target.draw(m_vertices, states);
	}

private:

	struct Particle
	{
		sf::Vector2f velocity;
		sf::Time lifetime;
	};

	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		float angle = (std::rand() % 360) * 3.14f / 180.f;
		float speed = (std::rand() % 50) + 50.f;
		m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

		// reset the position of the corresponding vertex
		m_vertices[index].position = m_emitter;
	}

	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
};



int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(512, 256), "Particles");

	// create the particle system
	ParticleSystem particles(1000);

	// create a clock to track the elapsed time
	sf::Clock clock;

	// run the main loop
	while (window.isOpen())
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// make the particle system emitter follow the mouse
		sf::Vector2i mouse = sf::Mouse::getPosition(window);
		particles.setTarget(window.mapPixelToCoords(mouse));

		// update it
		sf::Time elapsed = clock.restart();
		particles.update(elapsed);

		// draw it
		window.clear();
		window.draw(particles);
		window.display();
	}

	return 0;
}