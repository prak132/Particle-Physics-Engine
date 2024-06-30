const canvas = document.getElementById('simulationCanvas');
const ctx = canvas.getContext('2d');
const numParticles = parseInt(prompt("Amount of particles:"), 10);
const gravitationalConstant = parseFloat(prompt("Gravitational force in m/s² towards the center (example: 9.8 for Earth's gravity):")) * 100.0;
const viewWidth = canvas.width;
const viewHeight = canvas.height;
const gridSize = 20;
const friction = 0.99;
const particles = [];

let mouseParticle = null;
let lastMousePosition = { x: 0, y: 0 };

class Particle {
    constructor(x, y, radius, mass, velocityX, velocityY) {
        this.position = { x: x, y: y };
        this.velocity = { x: velocityX, y: velocityY };
        this.radius = radius;
        this.mass = mass;
        this.color = 'blue';
    }

    update(dt) {
        this.position.x += this.velocity.x * dt;
        this.position.y += this.velocity.y * dt;
        this.velocity.x *= friction;
        this.velocity.y *= friction;
        this.setColorBasedOnVelocity();
    }

    handleCollision(other) {
        let deltaX = other.position.x - this.position.x;
        let deltaY = other.position.y - this.position.y;
        let distance = Math.sqrt(deltaX * deltaX + deltaY * deltaY);
        if (distance < this.radius + other.radius && distance > 0.001) {
            let normalX = deltaX / distance;
            let normalY = deltaY / distance;
            let relativeVelocityX = other.velocity.x - this.velocity.x;
            let relativeVelocityY = other.velocity.y - this.velocity.y;
            let dot = relativeVelocityX * normalX + relativeVelocityY * normalY;
            let impulse = (2 * dot) / (this.mass + other.mass);
            this.velocity.x += impulse * normalX / this.mass;
            this.velocity.y += impulse * normalY / this.mass;
            other.velocity.x -= impulse * normalX / other.mass;
            other.velocity.y -= impulse * normalY / other.mass;
            let overlap = (this.radius + other.radius - distance) / 2;
            this.position.x -= overlap * normalX;
            this.position.y -= overlap * normalY;
            other.position.x += overlap * normalX;
            other.position.y += overlap * normalY;
        }
    }

    setColorBasedOnVelocity() {
        let speed = Math.sqrt(this.velocity.x * this.velocity.x + this.velocity.y * this.velocity.y);
        let maxSpeed = 200.0;
        let normalizedSpeed = Math.min(speed / maxSpeed, 1.0);
        let red = Math.floor(255 * normalizedSpeed);
        let blue = Math.floor(255 * (1 - normalizedSpeed));
        this.color = `rgb(${red}, 0, ${blue})`;
    }

    draw(ctx) {
        ctx.beginPath();
        ctx.arc(this.position.x, this.position.y, this.radius, 0, 2 * Math.PI);
        ctx.fillStyle = this.color;
        ctx.fill();
    }
}

for (let i = 0; i < numParticles; i++) {
    let x = Math.random() * viewWidth;
    let y = Math.random() * viewHeight;
    let radius = Math.random() * 7 + 3;
    let mass = Math.random() * 4 + 1;
    particles.push(new Particle(x, y, radius, mass, (Math.random() - 0.5) * 200, (Math.random() - 0.5) * 200));
}

canvas.addEventListener('mousemove', (event) => {
    const rect = canvas.getBoundingClientRect();
    const mouseX = event.clientX - rect.left;
    const mouseY = event.clientY - rect.top;

    if (!mouseParticle) {
        mouseParticle = new Particle(mouseX, mouseY, 10, 5, 0, 0);
        particles.push(mouseParticle);
    } else {
        mouseParticle.velocity.x = (mouseX - mouseParticle.position.x) * 10;
        mouseParticle.velocity.y = (mouseY - mouseParticle.position.y) * 10;
        mouseParticle.position.x = mouseX;
        mouseParticle.position.y = mouseY;
    }
});

document.getElementById('addParticleBtn').addEventListener('click', () => {
    const x = parseFloat(document.getElementById('newParticleX').value);
    const y = parseFloat(document.getElementById('newParticleY').value);
    const velocityX = parseFloat(document.getElementById('newParticleVelocityX').value);
    const velocityY = parseFloat(document.getElementById('newParticleVelocityY').value);
    let radius = Math.random() * 7 + 3;
    let mass = Math.random() * 4 + 1;
    particles.push(new Particle(x, y, radius, mass, velocityX, velocityY));
});

function update(dt) {
    particles.forEach(p => p.update(dt));

    // Boundary collisions
    particles.forEach(p => {
        if (p.position.x - p.radius < 0) {
            p.velocity.x *= -1;
            p.position.x = p.radius;
        }
        if (p.position.x + p.radius > viewWidth) {
            p.velocity.x *= -1;
            p.position.x = viewWidth - p.radius;
        }
        if (p.position.y - p.radius < 0) {
            p.velocity.y *= -1;
            p.position.y = p.radius;
        }
        if (p.position.y + p.radius > viewHeight) {
            p.velocity.y *= -1;
            p.position.y = viewHeight - p.radius;
        }
    });

    // Spatial partitioning
    let grid = new Map();
    particles.forEach(p => {
        let cellX = Math.floor(p.position.x / gridSize);
        let cellY = Math.floor(p.position.y / gridSize);
        let key = `${cellX},${cellY}`;
        if (!grid.has(key)) grid.set(key, []);
        grid.get(key).push(p);
    });

    grid.forEach((cellParticles, key) => {
        cellParticles.forEach((p1, i) => {
            for (let j = i + 1; j < cellParticles.length; j++) {
                let p2 = cellParticles[j];
                p1.handleCollision(p2);
            }

            let [cellX, cellY] = key.split(',').map(Number);
            for (let dx = -1; dx <= 1; dx++) {
                for (let dy = -1; dy <= 1; dy++) {
                    if (dx === 0 && dy === 0) continue;
                    let neighborKey = `${cellX + dx},${cellY + dy}`;
                    if (grid.has(neighborKey)) {
                        grid.get(neighborKey).forEach(p2 => p1.handleCollision(p2));
                    }
                }
            }
        });
    });

    let centralMass = { x: viewWidth / 2, y: viewHeight / 2 };
    particles.forEach(p => {
        let forceX = centralMass.x - p.position.x;
        let forceY = centralMass.y - p.position.y;
        let distance = Math.sqrt(forceX * forceX + forceY * forceY);
        if (distance > 0.001) {
            let force = (gravitationalConstant * p.mass) / (distance * distance);
            p.velocity.x += forceX * force * dt;
            p.velocity.y += forceY * force * dt;
        }
    });
}

function draw() {
    ctx.clearRect(0, 0, viewWidth, viewHeight);
    particles.forEach(p => p.draw(ctx));
}

let lastTime = 0;
function animate(time) {
    let dt = (time - lastTime) / 1000;
    lastTime = time;
    update(dt);
    draw();
    requestAnimationFrame(animate);
}

requestAnimationFrame(animate);
