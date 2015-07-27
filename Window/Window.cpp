
#include "Window.h"
#include "../State/State.h"
#include "../Core/Application.h"

Engine::Window *Engine::current_window = 0;

using namespace Engine;



Window::Window(){
	gameMain=0;
	mConsoleLineIndex=0;
	state=0;
	inputManager=0;
	application=0;
}
Window::~Window()
{
    current_window=0;
    state->Cleanup();
    delete state;
    delete inputManager;
}

void Window::setFocus()
{
    current_window = this;
}

void Window::setState(GameState* s){
	if(!s)return;
	state=s;
	state->Init();
}

void Window::setApplication(Application* app){
	application=app;
}

void Window::setGameMain(voidFunctor f){
	gameMain=f;
}

void Window::debugInput(){
	system("cls");
	for(int i=0;i<350;i++)cout<<input_pre.key[i]<<"-"<<input.key[i]<<"\t";
	
}

void Window::handleEvent(Event &msg){
	if(state){
		state->handleEvent(msg);
	}
	
}







/*
void Window::setup()
{
    setFocus();

    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 32;

    // Create the main window
	window.create(sf::VideoMode(800, 600, 32), "QEngine",sf::Style::Default,contextSettings);
    // Make it the active window.for OpenGL calls
    window.resetGLStates();
	window.setActive();
	window.setKeyRepeatEnabled(0);



	scene=new SceneManager();
	scene->Setup();
	scene->useDebugCamera();


	// Set the color and depth clear values
	glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // Disable lighting and texturing
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Configure the viewport (the same size as the window.
    glViewport(0, 0, window.getSize().x, window.getSize().y);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
    glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);


	if (!font.loadFromFile("Data/Resources/sansation.ttf"))
		error("cannot load the font\n\n");
	else debug("loaded the font\n\n");

	mConsoleText=sf::Text("", font);
	mConsoleText.setColor(sf::Color(255,0,0, 170));
	mConsoleText.setPosition(10,10);
	mConsoleText.setCharacterSize(10);


	debug("Setup done!");
}

int Window::exec()
{

    // Create a clock for measuring the time elapsed
    sf::Clock clock;

    // Start the game loop
    while(window.isOpen()) {
            // Process events
            sf::Event event;
            while(window.pollEvent(event)) {

                    switch(event.type) {
                            case sf::Event::Closed:
                                window.close();
                                break;
                            case sf::Event::TextEntered:
								{
								    if (event.text.unicode < 128){
						    			string s;
										s+=event.text.unicode;
										debug(s);
									}
								}break;
                            case sf::Event::KeyPressed:
                                if(event.key.code == sf::Keyboard::Escape)window.close();
                                if(event.key.code<0)break;
                                input.key[event.key.code] = 1;
                                break;
                            case sf::Event::KeyReleased:
                            	if(event.key.code<0)break;
                                input.key[event.key.code] = 0;
                                break;
                            case sf::Event::MouseMoved:
                                input.mouse.x = event.mouseMove.x;
                                input.mouse.y = event.mouseMove.y;
                                break;
                            case sf::Event::MouseButtonReleased:
                                switch(event.mouseButton.button) {
                                        case sf::Mouse::Left:
                                            input.mouse.button[MouseState::LEFT_BUTTON] = 0;
                                            break;
                                        case sf::Mouse::Right:
                                            input.mouse.button[MouseState::RIGHT_BUTTON] = 0;
                                            break;
                                        case sf::Mouse::Middle:
                                            input.mouse.button[MouseState::MIDDLE_BUTTON] = 0;
                                            break;
                                        default:break;
                                    }
                                setFocus();
                                break;
                            case sf::Event::MouseButtonPressed:
                                switch(event.mouseButton.button) {
                                        case sf::Mouse::Left:
                                            input.mouse.button[MouseState::LEFT_BUTTON] = 1;
                                            break;
                                        case sf::Mouse::Right:
                                            input.mouse.button[MouseState::RIGHT_BUTTON] = 1;
                                            break;
                                        case sf::Mouse::Middle:
                                            input.mouse.button[MouseState::MIDDLE_BUTTON] = 1;
                                            break;
                                        default:break;
                                    }
								setFocus();
                                break;
                            case sf::Event::Resized:
                                glViewport(0, 0, event.size.width, event.size.height);
                                window.setView(sf::View(sf::FloatRect(0,0, event.size.width, event.size.height)));
                                break;
                            case sf::Event::GainedFocus:
                            	setFocus();
                            	break;
                            default:
								break;
                        }
                }

			if(gameMain)gameMain();

            // Clear the color and depth buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Apply some transformations to rotate the cube
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();


            scene->Render(clock.getElapsedTime().asMicroseconds()/1000.0f);
			clock.restart();

			window.pushGLStates();
			window.draw(mConsoleText);
			window.popGLStates();

            // Finally, display the rendered frame on screen
            window.display();

            if(isKeyPressed(sf::Keyboard::Space))debug("key pressed");
            if(isKeyReleased(sf::Keyboard::Space))debug("key released");

            input_pre = input;
        }

    return EXIT_SUCCESS;
}

void Window::setMousePos(glm::vec2 pos)
{
	sf::Mouse::setPosition(sf::Vector2i(pos.x,pos.y),window);
}

void Window::appConsoleLine(string str)
{
	mConsoleLine[mConsoleLineIndex]=str;

	string ConsoleStr;
	for(int i=mConsoleLineIndex+1;i<mMaxNumberLineConsole;i++)ConsoleStr+=mConsoleLine[i];
	for(int i=0;i<=mConsoleLineIndex;i++)ConsoleStr+=mConsoleLine[i];

	mConsoleText.setString(ConsoleStr);

	mConsoleLineIndex++;
	if(mConsoleLineIndex==mMaxNumberLineConsole)mConsoleLineIndex=0;
}
*/



