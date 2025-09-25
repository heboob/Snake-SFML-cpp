#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <deque>
#include <random>
#include <string>
#include <cmath>

constexpr int WINDOW_WIDTH = 1024;
constexpr int WINDOW_HEIGHT = 1024;
constexpr int CELL_SIZE = 32;
constexpr int COLS = WINDOW_WIDTH / CELL_SIZE;
constexpr int ROWS = WINDOW_HEIGHT / CELL_SIZE;

enum Direction { Up, Down, Left, Right };
struct Segment { int x, y; };

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake Game SVI");
    window.setFramerateLimit(10);

    // Musik laden und starten
    sf::Music music;
    if (!music.openFromFile("startscreen.ogg")) return 1;
    music.setLoop(true);
    music.play();

    // Startscreen-Hintergrund
    sf::Texture startTexture;
    if (!startTexture.loadFromFile("startscreen.png")) return 1;
    sf::Sprite startSprite(startTexture);
    startSprite.setScale(
        float(WINDOW_WIDTH) / startTexture.getSize().x,
        float(WINDOW_HEIGHT) / startTexture.getSize().y
    );

    // Schriftart laden
    sf::Font font;
    if (!font.loadFromFile("PressStart2P.ttf")) return 1;

    // Titeltext oben
    sf::Text title("SNAKE GAME SVI", font, 32);
    title.setFillColor(sf::Color(255, 220, 100));
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.f, titleBounds.top + titleBounds.height / 2.f);
    title.setPosition(WINDOW_WIDTH / 2.f, 80.f);

    // Hinweistext Mitte
    sf::Text pressKey("Drücke eine Taste zum Starten", font, 20);
    pressKey.setFillColor(sf::Color(255, 220, 100));
    sf::FloatRect keyBounds = pressKey.getLocalBounds();
    pressKey.setOrigin(keyBounds.left + keyBounds.width / 2.f, keyBounds.top + keyBounds.height / 2.f);
    pressKey.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);

    // Credits unten
    sf::Text credits("Ein Spiel von Iheb Ben Kraiem und Tsu", font, 16);
    credits.setFillColor(sf::Color(200, 180, 80));
    sf::FloatRect creditBounds = credits.getLocalBounds();
    credits.setOrigin(creditBounds.left + creditBounds.width / 2.f, creditBounds.top + creditBounds.height / 2.f);
    credits.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 40.f);

    sf::Text scoreText("", font, 20);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setPosition(20.f, 20.f);

    sf::Text gameOverText("GAME OVER", font, 48);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(WINDOW_WIDTH / 2.f - 200.f, WINDOW_HEIGHT / 2.f - 50.f);

    bool started = false;
    bool gameOver = false;
    int score = 0;

    std::deque<Segment> snake{ {COLS / 2, ROWS / 2} };
    Direction dir = Right;

    std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> distX(0, COLS - 1);
    std::uniform_int_distribution<int> distY(0, ROWS - 1);
    Segment apple{ distX(rng), distY(rng) };

    // Startscreen mit Pochen-Effekt
    float pulseTime = 0.f;

    while (window.isOpen() && !started)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                started = true;
                music.stop(); // Musik beenden
            }
        }

        pulseTime += 0.1f;
        float scale = 1.f + 0.05f * std::sin(pulseTime);
        int brightness = 220 + static_cast<int>(35 * std::sin(pulseTime));
        sf::Color pulseColor(brightness, brightness - 20, 100);

        pressKey.setScale(scale, scale);
        pressKey.setFillColor(pulseColor);

        window.clear();
        window.draw(startSprite);
        window.draw(title);
        window.draw(pressKey);
        window.draw(credits);
        window.display();
    }

    // Hauptspiel-Loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!gameOver && event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::W && dir != Down)  dir = Up;
                if (event.key.code == sf::Keyboard::S && dir != Up)    dir = Down;
                if (event.key.code == sf::Keyboard::A && dir != Right) dir = Left;
                if (event.key.code == sf::Keyboard::D && dir != Left)  dir = Right;
            }
        }

        if (!gameOver)
        {
            Segment head = snake.front();
            switch (dir)
            {
            case Up:    head.y--; break;
            case Down:  head.y++; break;
            case Left:  head.x--; break;
            case Right: head.x++; break;
            }

            if (head.x < 0 || head.x >= COLS || head.y < 0 || head.y >= ROWS)
                gameOver = true;

            for (auto& s : snake)
                if (s.x == head.x && s.y == head.y)
                    gameOver = true;

            if (!gameOver)
            {
                snake.push_front(head);

                if (head.x == apple.x && head.y == apple.y)
                {
                    score++;
                    do {
                        apple = { distX(rng), distY(rng) };
                    } while (std::any_of(snake.begin(), snake.end(),
                        [&](const Segment& s) { return s.x == apple.x && s.y == apple.y; }));
                }
                else
                {
                    snake.pop_back();
                }
            }
        }

        // Zeichnen
        window.clear(sf::Color(30, 30, 30));

        if (gameOver)
        {
            window.draw(gameOverText);
            scoreText.setString("Punkte: " + std::to_string(score));
            scoreText.setPosition(WINDOW_WIDTH / 2.f - 80.f, WINDOW_HEIGHT / 2.f + 30.f);
            window.draw(scoreText);
        }
        else
        {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            cell.setFillColor(sf::Color::Red);
            cell.setPosition(apple.x * CELL_SIZE + 1, apple.y * CELL_SIZE + 1);
            window.draw(cell);

            cell.setFillColor(sf::Color::Green);
            for (auto& seg : snake)
            {
                cell.setPosition(seg.x * CELL_SIZE + 1, seg.y * CELL_SIZE + 1);
                window.draw(cell);
            }

            scoreText.setString("Punkte: " + std::to_string(score));
            scoreText.setPosition(20.f, 20.f);
            window.draw(scoreText);
        }

        window.display();
    }

    return 0;
}
