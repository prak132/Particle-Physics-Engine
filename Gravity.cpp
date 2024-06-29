#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>
#include <cmath>
#include <unordered_map>

int numParticles;
int viewWidth = 800, viewHeight = 600;
const float gravitationalConstant = 5000.0f;
const int gridSize = 20;

float dotProduct(const sf::Vector2f& v1, const sf::Vector2f& v2);

class Particle {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float radius;
    float mass;
    sf::CircleShape shape;

    Particle(float x, float y, float radius, float mass) : position(x, y), radius(radius), mass(mass) {
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::Blue);
    }

    void update(float dt) {
        position += velocity * dt;
        shape.setPosition(position);
        setColorBasedOnVelocity();
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

private:
    void setColorBasedOnVelocity() {
        float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        float maxSpeed = 200.0f;
        float normalizedSpeed = std::min(speed / maxSpeed, 1.0f);
        sf::Uint8 red = static_cast<sf::Uint8>(255 * normalizedSpeed);
        sf::Uint8 blue = static_cast<sf::Uint8>(255 * (1 - normalizedSpeed));
        shape.setFillColor(sf::Color(red, 0, blue));
    }
};

float dotProduct(const sf::Vector2f& v1, const sf::Vector2f& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

struct GridHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
    }
};

int main() {
    std::cout << "Amount of particles: ";
    std::cin >> numParticles;

    sf::RenderWindow window(sf::VideoMode(viewWidth, viewHeight), "Particle Physics Simulation");
    std::vector<Particle> particles;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDistX(0.0f, static_cast<float>(viewWidth));
    std::uniform_real_distribution<float> posDistY(0.0f, static_cast<float>(viewHeight));
    std::uniform_real_distribution<float> velDist(-100.0f, 100.0f);
    std::uniform_real_distribution<float> radiusDist(3.0f, 10.0f);
    std::uniform_real_distribution<float> massDist(1.0f, 5.0f);

    for (int i = 0; i < numParticles; i++) {
        float x = posDistX(gen);
        float y = posDistY(gen);
        float radius = radiusDist(gen);
        float mass = massDist(gen);
        Particle particle(x, y, radius, mass);
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

        for (std::size_t i = 0; i < particles.size(); i++) {
            particles[i].update(dt);

            if (particles[i].position.x - particles[i].radius < 0 || particles[i].position.x + particles[i].radius > viewWidth) {
                particles[i].velocity.x *= -1;
            }
            if (particles[i].position.y - particles[i].radius < 0 || particles[i].position.y + particles[i].radius > viewHeight) {
                particles[i].velocity.y *= -1;
            }
        }

        // Spatial partitioning
        std::unordered_map<std::pair<int, int>, std::vector<Particle*>, GridHash> grid;
        for (auto& particle : particles) {
            int cellX = static_cast<int>(particle.position.x / gridSize);
            int cellY = static_cast<int>(particle.position.y / gridSize);
            grid[{cellX, cellY}].push_back(&particle);
        }

        for (auto& cell : grid) {
            const auto& cellPos = cell.first;
            auto& cellParticles = cell.second;

            for (size_t i = 0; i < cellParticles.size(); i++) {
                Particle* p1 = cellParticles[i];

                for (size_t j = i + 1; j < cellParticles.size(); j++) {
                    Particle* p2 = cellParticles[j];
                    p1->handleCollision(*p2);
                }

                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        if (dx == 0 && dy == 0) continue;
                        auto neighborCell = grid.find({cellPos.first + dx, cellPos.second + dy});
                        if (neighborCell != grid.end()) {
                            for (auto& p2 : neighborCell->second) {
                                p1->handleCollision(*p2);
                            }
                        }
                    }
                }
            }
        }

        sf::Vector2f centralMass(viewWidth / 2.0f, viewHeight / 2.0f);
        for (auto& particle : particles) {
            sf::Vector2f forceDir = centralMass - particle.position;
            float distanceToCenter = sqrt(forceDir.x * forceDir.x + forceDir.y * forceDir.y);
            if (distanceToCenter > 0.001f) {
                sf::Vector2f force = (gravitationalConstant * particle.getMass() * distanceToCenter) /
                                     (distanceToCenter * distanceToCenter * distanceToCenter) * forceDir;
                particle.velocity += force * dt;
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
