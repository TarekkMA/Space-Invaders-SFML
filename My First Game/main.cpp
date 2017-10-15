#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"

//https://www.leshylabs.com/apps/sstool/


struct Collidable{
    
public:
    virtual int getWidth() = 0;
    virtual int getHigth() = 0;
    virtual int getX() = 0;
    virtual int getY() = 0;
    
    bool isCollided(Collidable&collidable){
        if(
           getX()              < collidable.getX() + collidable.getWidth() &&
           getX() + getWidth() > collidable.getX()                          &&
           getY()              < collidable.getY() + collidable.getHigth() &&
           getY() + getHigth() > collidable.getY()
           ){
            return true;
        }
        
        return false;
    }
};

template <typename T>
struct Node{
    T *item;
    Node<T> *next = NULL;
};

template <typename T>
struct LinkedList{
    
    Node<T> *head = NULL;
    Node<T> *tail = NULL;
    
    void deleteItem(T* item){
        
        Node<T> *current=new Node<T>;
        Node<T> *previous=new Node<T>;
        
        if(head->item == item){
            current = head;
            if(head->next == NULL){
                head = NULL;
                tail = NULL;
            }else{
                head = head->next;
                
            }
        
            return;
        }
        
        current=head;
        while(current->next!=NULL && current->item != item)
        {
            previous=current;
            current=current->next;
        }
        previous->next=current->next;
        delete current;
    }
    
    void addItem(T* item){
        Node<T> *temp = new Node<T>;
        temp->item = item;
        if(head == NULL){
            head = temp;
            tail = temp;
        }else{
            tail->next = temp;
            tail = temp;
        }
    }
    
    void forEach(std::function<void (T*)> func){
        Node<T> *temp=new Node<T>;
        temp=head;
        while(temp!=NULL)
        {
            func(temp->item);
            temp=temp->next;
        }
    }
};


struct Player : Collidable{
    int x;
    int y;
    sf::Sprite sprite;
    int dx = 20;
    int lives = 3;
    int score = 0;
    
    
    Player(const sf::Sprite &_sprite,int _x,int _y) : sprite(_sprite),x(_x),y(_y){
        sprite.setPosition(x, y);
    }
    
    
    int getWidth(){
        return sprite.getLocalBounds().width;
    }
    int getHigth(){
        return sprite.getLocalBounds().height;
    }
    int getX(){
        return x;
    }
    int getY(){
        return y;
    }
};

struct Aliean : Collidable{
    int x;
    int y;
    int worth = 10;
    sf::Sprite sprite;
    int dx = 5;
    float mult = 1;
    int dir = 1;
    Aliean(const sf::Sprite &_sprite,int _x,int _y) : sprite(_sprite),x(_x),y(_y){
        sprite.setPosition(x, y);
    }
    int getWidth(){
        return sprite.getLocalBounds().width;
    }
    int getHigth(){
        return sprite.getLocalBounds().height;
    }
    int getX(){
        return x;
    }
    int getY(){
        return y;
    }
};

struct Bullet : Collidable{
    int x;
    int y;
    sf::Sprite sprite;
    int dy = 20;
    
    Bullet(const sf::Sprite &_sprite,int _x,int _y) : sprite(_sprite),x(_x),y(_y){
        sprite.setPosition(x, y);
    }
    int getWidth(){
        return sprite.getLocalBounds().width;
    }
    int getHigth(){
        return sprite.getLocalBounds().height;
    }
    int getX(){
        return x;
    }
    int getY(){
        return y;
    }
};


bool loadSprites();
void renderBullets(Bullet* bullet,sf::RenderWindow* window);

const int WIDTH = 800;
const int HIGHT = 800;

const char* NAME = "Space Invaders";



int main(int, char const**)
{
    
    sf::RenderWindow *window;
    Player *player = NULL;
    LinkedList<Bullet> bulletList;
    LinkedList<Aliean> alieanList;
    
    
    
    sf::Texture spritesheet;
    sf::Sprite playerSprite;
    sf::Sprite alieanSprite1;
    sf::Sprite alieanSprite2;
    sf::Sprite alieanSprite3;
    sf::Sprite alieanSprite4;
    sf::Sprite alieanSprite5;
    sf::Sprite playerBulletSprite;
    
    int spwanPerTicks = 100;
    
    sf::Int64 ticks = 0 ;
    
    
    sf::Font font;
    sf::Text text;
    if (!font.loadFromFile(resourcePath() + "Pixelated.ttf"))
    {
        return EXIT_SUCCESS;
    }
    
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    
    if (!spritesheet.loadFromFile(resourcePath() + "spritesheet.png")) {
        return EXIT_SUCCESS;
    }
    
    playerSprite.setTexture(spritesheet);
    playerSprite.setTextureRect(sf::IntRect(333,229,73,52));
    
    playerBulletSprite.setTexture(spritesheet);
    playerBulletSprite.setTextureRect(sf::IntRect(387,405,16,42));
    
    alieanSprite1.setTexture(spritesheet);
    alieanSprite1.setTextureRect(sf::IntRect(0,0,110,80));
    
    alieanSprite2.setTexture(spritesheet);
    alieanSprite2.setTextureRect(sf::IntRect(333,0,80,80));
    
    alieanSprite3.setTexture(spritesheet);
    alieanSprite3.setTextureRect(sf::IntRect(212,0,120,80));
    
    alieanSprite4.setTexture(spritesheet);
    alieanSprite4.setTextureRect(sf::IntRect(223,324,80,80));
    
    alieanSprite5.setTexture(spritesheet);
    alieanSprite5.setTextureRect(sf::IntRect(263,405,123,57));
    
    
    // Create the main window
    window = new sf::RenderWindow(sf::VideoMode(WIDTH, HIGHT), NAME);

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return EXIT_FAILURE;
    }
    window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    
    player = new Player(playerSprite,WIDTH/2,HIGHT-playerSprite.getLocalBounds().height - 10);
    
    window->setFramerateLimit(30);
    
    // Start the game loop
    while (window->isOpen())
    {
        // Process events
        sf::Event event;
        while (window->pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window->close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window->close();
            }else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right) {
                if(player->x + player->dx + player->sprite.getGlobalBounds().width <= WIDTH){
                    player->x+=player->dx;
                    player->sprite.setPosition(player->x, player->y);
                }
            }else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left) {
                if(player->x - player->dx >= 0){
                    player->x-=player->dx;
                    player->sprite.setPosition(player->x, player->y);
                }
            }else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                
                Bullet* bullet = new Bullet(playerBulletSprite,player->x + player->sprite.getLocalBounds().width/2 -  playerBulletSprite.getLocalBounds().width /2,player->y);
                bulletList.addItem(bullet);
            }

        }

        // Clear screen
        window->clear();

        // Draw the sprite
        window->draw(player->sprite);
        
        
        
        
        bulletList.forEach([&](Bullet* bullet){
            
            bullet->y-=bullet->dy;
            bullet->sprite.setPosition(bullet->x, bullet->y);
            
            
            
            window->draw(bullet->sprite);
            
            //Check collision
            alieanList.forEach([&](Aliean* aliean){
                if(bullet->isCollided(*aliean)){
                    player->score+=aliean->worth;
                    bulletList.deleteItem(bullet);
                    alieanList.deleteItem(aliean);
                }
            });
            
        });
        
        
        alieanList.forEach([&](Aliean* aliean){
            aliean->x += aliean->dx * aliean->mult * aliean->dir;
            
            if(aliean->x > WIDTH || aliean->x < 0){
                aliean->dir*=-1;
                aliean->y += aliean->sprite.getLocalBounds().height;
            }
            
            aliean->sprite.setPosition(aliean->x, aliean->y);
            
            window->draw(aliean->sprite);
            
            if(aliean->y < 0){
                alieanList.deleteItem(aliean);
            }
        });
        
        
        
        
        if(ticks%spwanPerTicks == 0){
            Aliean* aliean;
            srand(time(NULL));
            int r = rand() % 100;//from 0 to 100
            std::cout << r << std::endl;
            if(r < 30){
                aliean = new Aliean(alieanSprite1,0,0);
            }else if(r < 55){
                aliean = new Aliean(alieanSprite2,0,0);
                aliean->worth = 25;
            }else if(r < 75){
                aliean = new Aliean(alieanSprite3,0,0);
                aliean->worth = 50;
            }else if(r < 90){
                aliean = new Aliean(alieanSprite4,0,0);
                aliean->worth = 50;
            }else if(r < 100){
                aliean = new Aliean(alieanSprite5,0,0);
                aliean->worth = 100;
            }
            alieanList.addItem(aliean);
        }
        
        text.setString(std::to_string(player->score));
        window->draw(text);
        
        // Update the window
        window->display();
        ticks++;
    }

    return EXIT_SUCCESS;
}

