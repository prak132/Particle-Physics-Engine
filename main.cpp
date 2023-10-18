#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

class Particle {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float radius;
    sf::CircleShape shape;

    Particle(float x, float y, float radius) : position(x, y), radius(radius) {
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::Blue);
    }

    void update(float dt) {
        position += velocity * dt;
        shape.setPosition(position);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Particle Physics Simulation");
    std::vector<Particle> particles;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> velDist(-100.0f, 100.0f);
    // Create initial particles
    for (int i = 0; i < 50; ++i) {
        float x = velDist(gen) + 400.0f;
        float y = velDist(gen) + 300.0f;
        float radius = 5.0f;
        Particle particle(x, y, radius);
        particle.velocity.x = velDist(gen);
        particle.velocity.y = velDist(gen);
        particles.push_back(particle);
    }
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        float dt = clock.restart().asSeconds();
        for (auto& particle : particles) {
            particle.update(dt);
            if (particle.position.x - particle.radius < 0 || particle.position.x + particle.radius > 800) {
                particle.velocity.x *= -1;
            }
            if (particle.position.y - particle.radius < 0 || particle.position.y + particle.radius > 600) {
                particle.velocity.y *= -1;
            }
        }
        window.clear(sf::Color::Black);
        for (const auto& particle : particles) {
            window.draw(particle.shape);
        }
        window.display();
    }
    return 0;
}
