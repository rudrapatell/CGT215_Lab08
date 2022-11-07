// CGT215_Lab08.cpp : This file contains the 'main' function. Program execution begins and ends there.


// Written by Rudra Patel on 5 November 2022

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}


int main()
{
    RenderWindow window(VideoMode(800, 600), "Duck Hunt");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400,
        600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle left;
    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true);
    world.AddPhysicsBody(left);

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    Texture redTex;
    LoadTex(redTex, "duck(3).png");
    PhysicsShapeList<PhysicsSprite> balloons;
    for (int i(0); i < 30; i++) {
        PhysicsSprite& balloon = balloons.Create();
        balloon.setTexture(redTex);
        balloon.setSize(Vector2f(100, 100));
        int x = 30 + ((750 / 5) * i);
        Vector2f sz = balloon.getSize();
        balloon.setCenter(Vector2f(x, 20 + (sz.y / 2)));
        balloon.setVelocity(Vector2f(0.25, 0));
        world.AddPhysicsBody(balloon);
        balloon.onCollision =
            [&drawingArrow, &world, &arrow, &balloon, &balloons, &score, &right]
        (PhysicsBodyCollisionResult result) {
            if (result.object2 == arrow) {
                drawingArrow = false;
                world.RemovePhysicsBody(arrow);
                world.RemovePhysicsBody(balloon);
                balloons.QueueRemove(balloon);
                score += 10;
            }
            if (result.object2 == right) {
                world.RemovePhysicsBody(balloon);
                balloons.QueueRemove(balloon);
            }
        };
    }

    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
    };

    Text scoreText;
    Font font;
    if (!font.loadFromFile("doodle.ttf")) {
        cout << "Couldn't load font arial.ttf" << endl;
        exit(1);
    }
    scoreText.setFont(font);
    Text arrowCountText;
    arrowCountText.setFont(font);

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);
    Time ducklastTime(lastTime);

    while ((arrows > 0) || drawingArrow) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            //MoveCrossbow(crossBow, deltaMS);
            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingArrow) {
                drawingArrow = true;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);
                arrows -= 1;
            }

            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            for (PhysicsShape& balloon : balloons) {
                window.draw((PhysicsSprite&)balloon);
            }
            window.draw(crossBow);
            scoreText.setString(to_string(score));
            FloatRect textBounds = scoreText.getGlobalBounds();
            scoreText.setPosition(
                Vector2f(790 - textBounds.width, 590 - textBounds.height));
            window.draw(scoreText);
            arrowCountText.setString(to_string(arrows));
            textBounds = arrowCountText.getGlobalBounds();
            arrowCountText.setPosition(
                Vector2f(10, 590 - textBounds.height));
            window.draw(arrowCountText);
            //world.VisualizeAllBounds(window);

            window.display();
            balloons.DoRemovals();
        }
        Time duckdeltaTime = currentTime - ducklastTime;
        long duckdeltaMS = duckdeltaTime.asMilliseconds();
        if (duckdeltaMS > 1000) {
            ducklastTime = currentTime;
            PhysicsSprite& balloon = balloons.Create();
            balloon.setTexture(redTex);
            Vector2f sz = balloon.getSize();
            balloon.setCenter(Vector2f(10, 20 + (sz.y / 2)));
            balloon.setVelocity(Vector2f(0.25, 0));
            world.AddPhysicsBody(balloon);
            balloon.onCollision =
                [&drawingArrow, &world, &arrow, &balloon, &balloons, &score, &right]
            (PhysicsBodyCollisionResult result) {
                if (result.object2 == arrow) {
                    drawingArrow = false;
                    world.RemovePhysicsBody(arrow);
                    world.RemovePhysicsBody(balloon);
                    balloons.QueueRemove(balloon);
                    score += 10;
                }
                if (result.object2 == right) {
                    world.RemovePhysicsBody(balloon);
                    balloons.QueueRemove(balloon);
                }
            };
        }
    }
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    FloatRect textBounds = gameOverText.getGlobalBounds();
    gameOverText.setPosition(Vector2f(
        400 - (textBounds.width / 2),
        300 - (textBounds.height / 2)
    ));
    window.draw(gameOverText);
    window.display();
    while (true);

