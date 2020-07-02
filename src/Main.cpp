#include "Res.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>

#include<Renderer.h>


#ifdef _WIN32
#include <Windows.h>
#define ERROR_MSG(x) MessageBox(nullptr, TEXT(x), TEXT("ERROR"), MB_OK);
#else
#define ERROR_MSG(x) std::cerr << x << std::endl;
#endif

struct InputState
{
	int frame = 0;
	bool isKeyPressed;
	bool keys[sf::Keyboard::KeyCount] = { false };
	bool key_press[sf::Keyboard::KeyCount] = { false };
	bool mouse[3] = { false };
	bool mouse_press[3] = { false };
	float wheel = 0.f;
	sf::Vector2f mouse_pos = sf::Vector2f(0, 0);
	sf::Vector2f mouse_press_pos = sf::Vector2f(0, 0);
	sf::Vector2f mouse_prev = sf::Vector2f(0, 0);
	sf::Vector2f mouse_speed = sf::Vector2f(0, 0);
	sf::Vector2f window_size = sf::Vector2f(0, 0);
	float time = 0, dt = 0;
	bool axis_moved[sf::Joystick::AxisCount] = { false };
	float axis_value[sf::Joystick::AxisCount] = { 0.f };
	bool  buttons[sf::Joystick::ButtonCount] = { false };
	bool  button_pressed[sf::Joystick::ButtonCount] = { false };
	std::string text_input;

	InputState(): mouse_pos(sf::Vector2f(0, 0)), mouse_speed(sf::Vector2f(0, 0))
	{
		std::fill(keys, keys + sf::Keyboard::KeyCount - 1, false);
		std::fill(mouse, mouse + 2, false);
	}
	InputState(bool a[sf::Keyboard::KeyCount], bool b[3], sf::Vector2f c, sf::Vector2f d):
		mouse_pos(c), mouse_speed(d)
	{
		std::copy(a, a + sf::Keyboard::KeyCount - 1, keys);
		std::copy(b, b + 2, mouse);
	}
};
InputState io_state;
bool fullscreen_current = false;
void InitializeWindow(sf::RenderWindow *window, bool fullscreen, float FPSlimit)
{
	if (!window->isOpen() || fullscreen != fullscreen_current)
	{
		fullscreen_current = fullscreen;

		sf::VideoMode screen_size;
		sf::Uint32 window_style;
		if (fullscreen) {
			screen_size = sf::VideoMode::getDesktopMode();
			window_style = sf::Style::Fullscreen;
		}
		else {
			screen_size = sf::VideoMode::getDesktopMode();
			window_style = sf::Style::Default;
		}

		//GL settings
		sf::ContextSettings settings;
		settings.majorVersion = 4;
		settings.minorVersion = 3;

		window->create(screen_size, "ShaderToy2CPP", window_style, settings);
		window->setVerticalSyncEnabled(true);
		window->setKeyRepeatEnabled(false);

		INIT();

		if (!fullscreen)
		{
			sf::VideoMode fs_size = sf::VideoMode::getDesktopMode();
			window->setSize(sf::Vector2u(fs_size.width/1.1, fs_size.height/1.1));
			window->setPosition(sf::Vector2i(0, 0));
		}
	}
	window->setFramerateLimit(FPSlimit);
}

void SetDefaultUniforms(Buffer* b)
{
	Shader* fv = b->GetShaderPtr();
	fv->setUniform("iResolution", vec3(io_state.window_size.x, io_state.window_size.y, 3.f));
	fv->setUniform("iTime", io_state.time);
	fv->setUniform("iTimeDelta", io_state.dt);
	fv->setUniform("iFrame", io_state.frame);
	fv->setUniform("iMouse", vec4(io_state.mouse_pos.x, io_state.mouse_pos.y,
								  io_state.mouse_press_pos.x, io_state.mouse_press_pos.y));
}

#if defined(_WIN32)
int WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpCmdLine, int nCmdShow) {
#else
int main(int argc, char *argv[]) {
#endif
	//window
	sf::RenderWindow window;

	//texture to which we will render the image
	sf::Texture main_txt;
	
	sf::Clock clock;

	InitializeWindow(&window, false, 60.);
	window.requestFocus();
  
	sf::View default_window_view = window.getDefaultView();

	Texture wolf0("textures/wolf_body.png");
	Texture wolf1("textures/eyes.png");
	Texture wolf2("textures/wolf_normal.png");
	Texture noise("textures/white_noise.png");

	float aspect_ratio = wolf2.GetSize().x / wolf2.GetSize().y;
	window.setSize(sf::Vector2u(720.f* aspect_ratio, 720.f));

	io_state.window_size = sf::Vector2f(window.getSize().x, window.getSize().y);
	main_txt.create(window.getSize().x, window.getSize().y);
	float prev_s = 1. / 60.;

	//BUFFERS
	Texture maintxt(&main_txt);

	Buffer main(&maintxt);
	main.SetShader("shaders/Wolf.glsl");
	main.SetInput(0, &wolf0);
	main.SetInput(1, &wolf1);
	main.SetInput(2, &wolf2);
	main.SetInput(3, &noise);

	io_state.frame = 0;

	//Main loop
	bool isopen = true;
	while (isopen)
	{
		sf::Event event;
		window.clear(sf::Color::Blue);

		float mouse_wheel = 0.0f;
		io_state.mouse_prev = io_state.mouse_pos;
		io_state.wheel = mouse_wheel;
		io_state.mouse_press[2] = false;
		io_state.mouse_press[0] = false;
		io_state.text_input = "";

		for (int i = 0; i < sf::Keyboard::KeyCount; i++)
		{
			io_state.key_press[i] = false;
		}
		for (int i = 0; i < sf::Joystick::AxisCount; i++)
		{
			io_state.axis_moved[i] = false;
		}
		for (int i = 0; i < sf::Joystick::ButtonCount; i++)
		{
			io_state.button_pressed[i] = false;
		}

		while (window.pollEvent(event)) 
		{
			sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
			const sf::Keyboard::Key keycode = event.key.code;
			switch (event.type)
			{
			case sf::Event::Closed:
				isopen = false;
				break;
			case sf::Event::Resized:
				default_window_view = sf::View(visibleArea);
				window.setView(default_window_view);
				io_state.window_size = sf::Vector2f(window.getSize().x, window.getSize().y);
				break;
			case sf::Event::JoystickButtonPressed:
				io_state.buttons[event.joystickButton.button] = true;
				io_state.button_pressed[event.joystickButton.button] = true;
				break;
			case sf::Event::JoystickButtonReleased:
				io_state.buttons[event.joystickButton.button] = false;
				break;
			case sf::Event::JoystickMoved:
				io_state.axis_value[event.joystickMove.axis] =
					(abs(event.joystickMove.position) < 0.05) ? 0.f : event.joystickMove.position;
				io_state.axis_moved[event.joystickMove.axis] = true;
				break;
			case sf::Event::TextEntered:
				io_state.text_input = event.text.unicode;
				break;
			case sf::Event::KeyPressed:
				io_state.isKeyPressed = true;
				io_state.keys[keycode] = true;
				io_state.key_press[keycode] = true;
				break;
			case sf::Event::KeyReleased:
				if (event.key.code < 0 || event.key.code >= sf::Keyboard::KeyCount) { continue; }
				io_state.keys[keycode] = false;
				break;
			case sf::Event::MouseButtonPressed:
				io_state.mouse_press_pos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					io_state.mouse[0] = true;
					io_state.mouse_press[0] = true;
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					io_state.mouse[2] = true;
					io_state.mouse_press[2] = true;
				}
				break;
			case sf::Event::MouseButtonReleased:
				io_state.mouse_press_pos = sf::Vector2f(0, 0);
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					io_state.mouse[0] = false;
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					io_state.mouse[2] = false;
				}
				break;
			case sf::Event::MouseMoved:
				io_state.mouse_pos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
				break;
			case sf::Event::MouseWheelScrolled:
				mouse_wheel += event.mouseWheelScroll.delta;
				io_state.wheel = mouse_wheel;
				break;
			}
		}

		//set default uniforms for all buffers
		SetDefaultUniforms(&main);
		main.Render();

		//reset the GL state for SFML to work properly
		window.resetGLStates();

		//Draw render texture to main window
		sf::Sprite sprite(main_txt);
		sprite.setScale(float(window.getSize().x) / float(main_txt.getSize().x),
			-float(window.getSize().y) / float(main_txt.getSize().y));
		sprite.setPosition(0., window.getSize().y);
		window.draw(sprite);

		//update frame parameters
		io_state.frame += 1;
		io_state.dt = prev_s;
		io_state.time += io_state.dt;
		io_state.isKeyPressed = false;

		//update window
		window.setView(default_window_view);
		window.display();
	}

	main_txt.copyToImage().saveToFile("result.png");

	return 0;
}
