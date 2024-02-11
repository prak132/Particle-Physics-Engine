#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include <vector>
#include <random>
#include <iostream>
#include <memory>
#include <unordered_map>

int numParticles;
int viewWidth = 1200, viewHeight = 800;
int maxFPS = -1;

float dotProduct(const sf::Vector2f& v1, const sf::Vector2f& v2);

class Particle {
public:
    glm::vec2 position;
    glm::vec2 velocity;
    float radius;
    sf::CircleShape shape;

    Particle(float x, float y, float radius)
        : position(x, y), velocity(0.0f, 0.0f), radius(radius) {
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(position.x, position.y);
        shape.setFillColor(sf::Color::Blue);
    }

    void update(float dt) {
        position += velocity * dt;
        for (int i = 0; i < 2; ++i) {
            float& pos = i == 0 ? position.x : position.y;
            float& vel = i == 0 ? velocity.x : velocity.y;
            const float boundaryMin = radius;
            const float boundaryMax = i == 0 ? viewWidth - radius : viewHeight - radius;

            if (pos < boundaryMin) {
                vel *= -1;
                pos = boundaryMin;
            } else if (pos > boundaryMax) {
                vel *= -1;
                pos = boundaryMax;
            }
        }
        shape.setPosition(position.x, position.y);
    }

    void handleCollision(Particle& other) {
        glm::vec2 delta = other.position - position;
        float distance = glm::length(delta);
        if (distance < radius + other.radius && distance > 0.001f) {
            glm::vec2 normal = glm::normalize(delta);
            glm::vec2 relativeVelocity = other.velocity - velocity;
            float impulse = (2.0f * glm::dot(relativeVelocity, normal)) / (2.0f);
            velocity += impulse * normal;
            other.velocity -= impulse * normal;
            float overlap = 0.5f * (radius + other.radius - distance);
            position -= overlap * normal;
            other.position += overlap * normal;
        }
    }
};

float dotProduct(const sf::Vector2f& v1, const sf::Vector2f& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

class Grid {
public:
    struct CellKey {
        int x, y;
        bool operator==(const CellKey& other) const {
            return x == other.x && y == other.y;
        }
    };
    struct CellKeyHash {
        size_t operator()(const CellKey& key) const {
            return std::hash<int>()(key.x) ^ (std::hash<int>()(key.y) << 1);
        }
    }; std::unordered_map<CellKey, std::vector<std::shared_ptr<Particle>>, CellKeyHash> cells;
    int cellSize;
    Grid(int size) : cellSize(size) {}
    void clear() {
        cells.clear();
    }
    void addParticle(const std::shared_ptr<Particle>& particle) {
        CellKey key = {int(particle->position.x / cellSize), int(particle->position.y / cellSize)};
        cells[key].push_back(particle);
    }
    std::vector<std::shared_ptr<Particle>> getNearbyParticles(const std::shared_ptr<Particle>& particle) {
        std::vector<std::shared_ptr<Particle>> nearby;
        int x = particle->position.x / cellSize;
        int y = particle->position.y / cellSize;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                CellKey key = {x + dx, y + dy};
                auto it = cells.find(key);
                if (it != cells.end()) {
                    nearby.insert(nearby.end(), it->second.begin(), it->second.end());
                }
            }
        } return nearby;
    }
};

int main() {
    std::cout << "Amount of particles: ";
    std::cin >> numParticles;
    sf::RenderWindow window(sf::VideoMode(viewWidth, viewHeight), "Particle Physics Simulation");
    std::vector<std::shared_ptr<Particle>> particles;
    Grid grid(50); // more for better accuracy and less for better performance 50 is nice balance
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> velDist(-100.0f, 100.0f);
    std::uniform_real_distribution<float> radiusDist(4.0f, 10.0f);
    sf::Font font;
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10.f, 10.f);
    sf::Clock fpsClock;
    float lastTime = 0.0;
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Could not load font\n";
        return 1;
    } for (int i = 0; i < numParticles; i++) {
        float x = velDist(gen) + viewWidth / 2.0f;
        float y = velDist(gen) + viewHeight / 2.0f;
        float radius = radiusDist(gen);
        auto particle = std::make_shared<Particle>(x, y, radius);
        particle->velocity.x = velDist(gen);
        particle->velocity.y = velDist(gen);
        particles.push_back(particle);
    } sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        } float dt = clock.restart().asSeconds();
        window.clear(sf::Color::Black);
        grid.clear();
        float currentTime = fpsClock.restart().asSeconds();
        float fps = 1.f / currentTime;
        lastTime = currentTime;
        fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
        for (auto& particle : particles) {
            particle->update(dt);
            grid.addParticle(particle);
        } window.clear(sf::Color::Black);
        for (const auto& particle : particles) {
            window.draw(particle->shape);
        } for (auto& particle : particles) {
            auto nearbyParticles = grid.getNearbyParticles(particle);
            for (auto& other : nearbyParticles) {
                if (particle != other) {
                    particle->handleCollision(*other);
                }
            }
        } for (const auto& particle : particles) {
            window.draw(particle->shape);
        } window.draw(fpsText);
        window.display();
        maxFPS = std::max(maxFPS, static_cast<int>(fps));
    } std::cout << "Max FPS reached: " << maxFPS << "\n";
    return 0;
}
