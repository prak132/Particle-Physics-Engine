#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>

int numParticles;
int viewWidth = 800, viewHeight = 600;
const float gravitationalConstant = 5000.0f; // use large numbers for interesting reactions
float dotProduct(const sf::Vector2f& v1, const sf::Vector2f& v2);

class Particle {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float radius;
    sf::CircleShape shape;

    Particle(float x, float y, float radius) : position(x, y), radius(radius), mass(1.0f) {
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::Blue);
    }

    void update(float dt) {
        position += velocity * dt;
        shape.setPosition(position);
    }

    void handleCollision(Particle& other) {
        sf::Vector2f delta = other.position - position;
        float distance = sqrt(delta.x * delta.x + delta.y * delta.y);
        if (distance < radius + other.radius && distance > 0.001f) {
            sf::Vector2f normal = delta / distance;
            sf::Vector2f relativeVelocity = other.velocity - velocity;
            float impulse = (2.0f * dotProduct(relativeVelocity, normal)) / (1.0f / mass + 1.0f / other.mass);
            velocity += impulse * normal / mass;
            other.velocity -= impulse * normal / other.mass;
            float overlap = (radius + other.radius - distance) / 2.0f;
            position -= overlap * normal;
            other.position += overlap * normal;
        }
    }

    float getMass() const {
        return mass;
    }

// greater mass = more inertia
private:
    float mass = 1.0f;
};

float dotProduct(const sf::Vector2f& v1, const sf::Vector2f& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

int main() {
    std::cout << "Amt of particles: ";
    std::cin >> numParticles;
    sf::RenderWindow window(sf::VideoMode(viewWidth, viewHeight), "Particle Physics Simulation");
    std::vector<Particle> particles;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> velDist(-100.0f, 100.0f);
    for (int i = 0; i < numParticles; i++) {
        float x = velDist(gen) + 400.0f;
        float y = velDist(gen) + 300.0f;
        float radius = 5.0f;
        Particle particle(x, y, radius);
        particle.velocity.x = velDist(gen);
        particle.velocity.y = velDist(gen);
        particles.push_back(particle);
    } sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        } float dt = clock.restart().asSeconds();
        for (std::size_t i = 0; i < particles.size(); i++) {
            particles[i].update(dt);
            if (particles[i].position.x - particles[i].radius < 0 || particles[i].position.x + particles[i].radius > viewWidth) {
                particles[i].velocity.x *= -1;
            } if (particles[i].position.y - particles[i].radius < 0 || particles[i].position.y + particles[i].radius > viewHeight) {
                particles[i].velocity.y *= -1;
            } for (std::size_t j = i + 1; j < particles.size(); j++) {
                particles[i].handleCollision(particles[j]);
            }
        } sf::Vector2f centralMass(viewWidth / 2.0f, viewHeight / 2.0f);
        for (std::size_t i = 0; i < particles.size(); i++) {
            sf::Vector2f forceDir = centralMass - particles[i].position;
            float distanceToCenter = sqrt(forceDir.x * forceDir.x + forceDir.y * forceDir.y);
            sf::Vector2f force = (gravitationalConstant * particles[i].getMass() * distanceToCenter) / (distanceToCenter * distanceToCenter * distanceToCenter) * forceDir;
            particles[i].velocity += force * dt;
        } window.clear(sf::Color::Black);
        for (const auto& particle : particles) {
            window.draw(particle.shape);
        } window.display();
    } return 0;
}
