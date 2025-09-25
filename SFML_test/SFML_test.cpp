#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

// Spielfeld-Konfiguration
const int CELL_SIZE = 20;
const int GRID_WIDTH = 30;
const int GRID_HEIGHT = 20;
const int WINDOW_WIDTH = GRID_WIDTH * CELL_SIZE;
const int WINDOW_HEIGHT = GRID_HEIGHT * CELL_SIZE;
const int FRAME_RATE = 10;

enum Direction { Up, Down, Left, Right };

struct Segment {
    int x, y;
    Segment(int x, int y) : x(x), y(y) {}
};

class Snake {
public:
    Snake() {
        reset();
    }

    void reset() {
        segments.clear();
        segments.push_back(Segment(GRID_WIDTH / 2, GRID_HEIGHT / 2));
        dir = Right;
        growNext = false;
    }

    void setDirection(Direction newDir) {
        if ((dir == Up && newDir == Down) || (dir == Down && newDir == Up) ||
            (dir == Left && newDir == Right) || (dir == Right && newDir == Left))
            return;
        dir = newDir;
    }

    void move() {
        Segment head = segments[0];
        switch (dir) {
        case Up:    head.y--; break;
        case Down:  head.y++; break;
        case Left:  head.x--; break;
        case Right: head.x++; break;
        }

        segments.insert(segments.begin(), head);

        if (growNext)
            growNext = false;
        else
            segments.pop_back();
    }

    void grow() {
        growNext = true;
    }

    bool checkCollision() {
        const Segment& head = segments[0];

        // Wandkollision
        if (head.x < 0 || head.x >= GRID_WIDTH || head.y < 0 || head.y >= GRID_HEIGHT)
            return true;

        // Selbstkollision
        for (size_t i = 1; i < segments.size(); ++i) {
            if (head.x == segments[i].x && head.y == segments[i].y)
                return true;
        }

        return false;
    }

    const std::vector<Segment>& getSegments() const {
        return segments;
    }

    Segment getHead() const {
        return segments[0];
    }

private:
    std::vector<Segment> segments;
    Direction dir;
    bool growNext;
};

sf::Vector2i generateFood(const Snake& snake) {
    sf::Vector2i food;
    bool onSnake;
    do {
        food.x = std::rand() % GRID_WIDTH;
        food.y = std::rand() % GRID_HEIGHT;
        onSnake = false;
        for (const auto& segment : snake.getSegments()) {
            if (segment.x == food.x && segment.y == food.y) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
    return food;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Snake Game");
    window.setFramerateLimit(FRAME_RATE);

    sf::Font font;
    if (!font.openFromFile("arial.ttf"))
    {
        return 1;  // Stelle sicher, dass eine Schriftart vorhanden ist
    }

    Snake snake;
    sf::Vector2i food = generateFood(snake);
    int score = 0;
    bool gameOver = false;

    /* Das habe ich geändert Tsoa haha!!!
        while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }*/
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (!gameOver) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up)) snake.setDirection(Up);
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down)) snake.setDirection(Down);
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) snake.setDirection(Left);
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) snake.setDirection(Right);

                snake.move();

                if (snake.getHead().x == food.x && snake.getHead().y == food.y) {
                    snake.grow();
                    food = generateFood(snake);
                    score += 10;
                }

                if (snake.checkCollision()) {
                    gameOver = true;
                }
            }
            else {
                // Neustart mit Enter
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Enter)) {
                    snake.reset();
                    food = generateFood(snake);
                    score = 0;
                    gameOver = false;
                }
            }

            // Zeichnen
            window.clear(sf::Color::Black);

            // Futter zeichnen
            sf::RectangleShape foodRect(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            foodRect.setFillColor(sf::Color::Red);
            foodRect.setPosition({ food.x * CELL_SIZE + 1.f, food.y * CELL_SIZE + 1.f });
            window.draw(foodRect);

            // Snake zeichnen
            sf::RectangleShape segmentRect(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            segmentRect.setFillColor(sf::Color::Green);
            for (const auto& segment : snake.getSegments()) {
                segmentRect.setPosition({ segment.x * CELL_SIZE + 1.f, segment.y * CELL_SIZE + 1.f });
                window.draw(segmentRect);
            }

            // Score
            sf::Text scoreText(font);
            scoreText.setFont(font);
            scoreText.setCharacterSize(20);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setString("Score: " + std::to_string(score));
            scoreText.setPosition({ 5, 5 });
            window.draw(scoreText);

            // Game Over Text
            if (gameOver) {
                sf::Text goText(font);
                goText.setFont(font);
                goText.setCharacterSize(36);
                goText.setFillColor(sf::Color::Red);
                goText.setString("Game Over!\nEnter = Restart");
                {
                    goText.setPosition({ WINDOW_WIDTH / 2 - 130, WINDOW_HEIGHT / 2 - 50 });
                    window.draw(goText);
                }

                window.display();
            }

           
        }
    }return 0;
}
