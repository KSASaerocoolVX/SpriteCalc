module;

#include <iostream>
#include <string>
#include <SFML/Graphics.hpp> 
#include <cmath>
#include <memory>
#include <sstream>

module CalculatorUI;

import parser.core;
import core.context;
import core.exceptions;

import InputScreen;
import TextNode;
import AssetManager;
import MathRow;
import FractionNode;
import ExponentNode;
import MathEditor;
import app.help;
import CalculatorUI;


static std::string wrapText(const std::string& text, std::size_t maxCharsLine) {
    std::string result;
    std::size_t start = 0;
    while (start < text.size()) {
        std::size_t end = text.find('\n', start);
        if (end == std::string::npos) {
            end = text.size();
        }
        std::string line = text.substr(start, end - start);
        if (line.empty()) {
            result += "\n";
            start = end + 1;
            continue;
        }
        std::size_t lineStart = 0;
        bool first = true;
        while (lineStart < line.size()) {
            std::size_t limit = first ? maxCharsLine : (maxCharsLine > 2 ? maxCharsLine - 2 : maxCharsLine);
            if (line.size() - lineStart <= limit) {
                if (!first) result += "  ";
                result += line.substr(lineStart) + "\n";
                break;
            }
            std::size_t space = line.rfind(' ', lineStart + limit);
            if (space == std::string::npos || space < lineStart) {
                if (!first) result += "  ";
                result += line.substr(lineStart, limit) + "\n";
                lineStart += limit;
            } else {
                if (!first) result += "  ";
                result += line.substr(lineStart, space - lineStart) + "\n";
                lineStart = space + 1;
            }
            first = false;
        }
        start = end + 1;
    }
    return result;
}

std::string intToLabel(int index)
{
	switch (index)
	{
	case 0: return "AC";
	case 1: return "DEL";
	case 2: return "P";
	case 3: return "/";
	case 4: return "7";
	case 5: return "8";
	case 6: return "9";
	case 7: return "*";
	case 8: return "4";
	case 9: return "5";
	case 10: return "6";
	case 11: return "-";
	case 12: return "1";
	case 13: return "2";
	case 14: return "3";
	case 15: return "+";
	case 16: return "<";
	case 17: return "0";
	case 18: return ">";
	case 19: return "=";
	case 20: return ".";
	case 21: return "(";
	case 22: return ")";
	case 23: return "%";
	default: return "P";
	}
}

CalculatorUI::CalculatorUI() : m_sprite(AssetManager::Instance().GetTexture("UI/assets/calculator_emptyExtended.png"))
{

	sf::Vector2u textureSize = m_sprite.getTexture().getSize();

	auto infoBtn = std::make_unique<Button>(
		sf::Vector2f(0.0f, 0.0f),
		"UI/assets/infoButton.png",
		"UI/assets/infoButton.png",
		""
	);
	infoBtn->onClick = [this]() {
		this->ShowHelpWindow();
	};
	children.push_back(std::move(infoBtn));

	//main buttons

	float texWidth = static_cast<float>(textureSize.x);
	float texHeight = static_cast<float>(textureSize.y);

	float offsetY = texHeight * 0.325f;
	float offsetX = texWidth * 0.11f;

	float paddingX = texWidth * 0.028f;
	float paddingY = texHeight * 0.02f;

	sf::Vector2u buttonSize = AssetManager::Instance().GetTexture("UI/assets/button_0.png").getSize();

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float stepX = (buttonSize.x + paddingX) * j;
			float stepY = (buttonSize.y + paddingY) * i;
			auto localPosition = sf::Vector2f(offsetX + stepX, offsetY + stepY);

			std::string label = intToLabel((i * 4) + j);

			auto button = std::make_unique<Button>(
				localPosition,
				"UI/assets/button_0.png",
				"UI/assets/button_1.png",
				label
			);

			button->onClick = [this, label]() {
				this->HandleButtonPress(label);
				};

			children.push_back(std::move(button));
		}
	}

	//вкладки
	float tabStartX = texWidth * 0.925f;
	float tabStartY = texHeight * 0.1f;

	float handleW = 25.0f; 
	float handleH = 34.0f; 
	float handleY = 14.0f; 

	auto trigTab = std::make_unique<CalculatorUITab>(
		sf::Vector2f(tabStartX, texHeight * 0.1f),
		"UI/assets/trig_tab.png",
		handleW,
		handleH,
		handleY,
		"" 
	);

	auto btnSin = std::make_unique<Button>(sf::Vector2f(offsetX, 15.f), "UI/assets/button_0.png", "UI/assets/button_1.png", "sin");
	btnSin->onClick = [this]() { HandleButtonPress("sin"); };

	auto btnCos = std::make_unique<Button>(sf::Vector2f(offsetX+ (buttonSize.x + paddingX), 15.f), "UI/assets/button_0.png", "UI/assets/button_1.png", "cos");
	btnCos->onClick = [this]() { HandleButtonPress("cos"); };

	auto btnTg = std::make_unique<Button>(sf::Vector2f(offsetX + (buttonSize.x + paddingX)*2, 15.f), "UI/assets/button_0.png", "UI/assets/button_1.png", "tan");
	btnTg->onClick = [this]() { HandleButtonPress("tan"); };

	trigTab->AddButton(std::move(btnSin));
	trigTab->AddButton(std::move(btnCos));
	trigTab->AddButton(std::move(btnTg));

	auto linalTab = std::make_unique<CalculatorUITab>(
		sf::Vector2f(tabStartX, texHeight * 0.3f),
		"UI/assets/linal_tab.png",
		handleW,
		handleH,
		handleY,
		"" 
	);

	auto discrTab = std::make_unique<CalculatorUITab>(
		sf::Vector2f(tabStartX, texHeight * 0.5f),
		"UI/assets/discr_tab.png",
		handleW,
		handleH,
		handleY,
		"" 
	);

	m_tabs.push_back(std::move(trigTab));
	m_tabs.push_back(std::move(linalTab));
	m_tabs.push_back(std::move(discrTab));

	//экран
	float screenOffsetY = texHeight * 0.1f;
	float screenOffsetX = texWidth * 0.08f;
	auto screenPos = sf::Vector2f(screenOffsetX, screenOffsetY);

	auto screen = std::make_unique<InputScreen>(screenPos, "UI/assets/input_screen.png");
	m_screenRef = screen.get();



	children.push_back(std::move(screen));
}

void CalculatorUI::HandleButtonPress(const std::string& label)
{
	if (label != "=")
	{
		if (m_lastAnswer != nullptr)
		{
			if (label != "+" && label != "-" && label != "*" && label != "/" && label != "P" && label != "%" &&
				label != "<" && label != ">" && label != "DEL" && label != "AC")
			{
				m_editor.Clear();
			}
		}
		m_lastAnswer = nullptr;
		if (m_screenRef)
		{
			m_screenRef->SetOutput(nullptr);
		}
	}

	if (label == "AC") m_editor.Clear();
	else if (label == "/") m_editor.InsertFraction();
	else if (label == "P") m_editor.InsertExponent();
	else if (label == "<") m_editor.MoveLeft();
	else if (label == ">") m_editor.MoveRight();
	else if (label == "DEL") m_editor.Delete();
	else if (label == "+" || label == "-" || label == "*" || label == "=" || label == "%")
	{
		if (label == "=" && m_screenRef) {
			IMathNode* root = static_cast<IMathNode*>(m_editor.GetRoot());
			if (m_screenRef && root)
			{
				try {
					std::string expression = root->ToString();
					std::cout << "got expression: " << expression << std::endl;

					std::string trimmedExpr = expression;
					while (!trimmedExpr.empty() && std::isspace(static_cast<unsigned char>(trimmedExpr.front()))) {
						trimmedExpr.erase(trimmedExpr.begin());
					}
					while (!trimmedExpr.empty() && std::isspace(static_cast<unsigned char>(trimmedExpr.back()))) {
						trimmedExpr.pop_back();
					}
					std::string lowerExpr = trimmedExpr;
					for (auto& c : lowerExpr) c = std::tolower(static_cast<unsigned char>(c));

					if (lowerExpr == "help" || lowerExpr.rfind("help ", 0) == 0) {
						std::string topic = "";
						if (lowerExpr.rfind("help ", 0) == 0) {
							topic = trimmedExpr.substr(5);
							while (!topic.empty() && std::isspace(static_cast<unsigned char>(topic.front()))) {
								topic.erase(topic.begin());
							}
							while (!topic.empty() && std::isspace(static_cast<unsigned char>(topic.back()))) {
								topic.pop_back();
							}
							for (auto& c : topic) c = std::tolower(static_cast<unsigned char>(c));
						}

						std::stringstream ss;
						app::printHelp(ss, topic);
						m_helpTextStr = wrapText(ss.str(), 50);

						if (!m_helpWindow || !m_helpWindow->isOpen())
						{
							m_helpWindow = std::make_unique<sf::RenderWindow>(sf::VideoMode({600, 650}), "Help - SpriteCalc");
						}
						m_helpWindow->requestFocus();

						m_lastAnswer = std::make_unique<TextNode>("[Help Opened]", 56, sf::Color(50, 150, 50));
						m_screenRef->SetOutput(m_lastAnswer.get());
					} else {
						parser::Parser calcParser;
						core::Context ctx;
						auto statement = calcParser.parse(expression);
						core::Value result = statement.evaluate(ctx);

						std::string answerStr = "= " + result.toString();
						std::cout << "got answer string: " << answerStr << std::endl;

						m_lastAnswer = std::make_unique<TextNode>(answerStr, 56, sf::Color(50, 150, 50));
						m_screenRef->SetOutput(m_lastAnswer.get());
					}
				}
				catch (const core::SyntaxError& e)
				{
					std::cout << "Syntax Error: " << e.what() << std::endl;

					m_lastAnswer = std::make_unique<TextNode>("Syntax Error", 36, sf::Color(200, 50, 50));
					m_screenRef->SetOutput(m_lastAnswer.get());
				}
				catch (const core::MathError& e)
				{
					std::cout << "Math Error: " << e.what() << std::endl;
					m_lastAnswer = std::make_unique<TextNode>("Math Error", 36, sf::Color(200, 50, 50));
					m_screenRef->SetOutput(m_lastAnswer.get());
				}
				catch (const core::CalcError& e)
				{
					std::cout << "Error: " << e.what() << std::endl;
					m_lastAnswer = std::make_unique<TextNode>("Error", 36, sf::Color(200, 50, 50));
					m_screenRef->SetOutput(m_lastAnswer.get());
				}
				catch (const std::exception& e)
				{
					std::cout << "Unexpected Exception: " << e.what() << std::endl;
					m_lastAnswer = std::make_unique<TextNode>("Syntax Error", 36, sf::Color(200, 50, 50));
					m_screenRef->SetOutput(m_lastAnswer.get());
				}
			}
		}
		else {
			m_editor.InsertOperator(" " + label + " "); 
		}
	}
	else {
		m_editor.InsertDigit(label);
	}

	IMathNode* root_bottom = static_cast<IMathNode*>(m_editor.GetRoot());
	root_bottom->Measure();
	root_bottom->Arrange();

	if (m_screenRef) {
		m_screenRef->SetInput(root_bottom);
	}

	std::cout << "buffer: \"" << m_inputBuffer << "\"\n";
}

void CalculatorUI::Update(float deltaTime, sf::Vector2f mousePos)
{
	m_editor.Update(deltaTime);

	sf::Vector2f localMousePos = getInverseTransform().transformPoint(mousePos);
	for (auto& child : children)
	{
		child->Update(deltaTime, localMousePos);
	}

	for (auto& tab : m_tabs) {
		tab->Update(deltaTime, localMousePos);
	}

	if (m_helpWindow && m_helpWindow->isOpen())
	{
		while (const std::optional event = m_helpWindow->pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				m_helpWindow->close();
			}
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->code == sf::Keyboard::Key::Escape)
				{
					m_helpWindow->close();
				}
				else
				{
					HandleEvent(*event, *m_helpWindow);
				}
			}
			else if (event->getIf<sf::Event::TextEntered>())
			{
				HandleEvent(*event, *m_helpWindow);
			}
		}

		m_helpWindow->clear(sf::Color(30, 30, 30));
		
		auto& font = AssetManager::Instance().GetFont("UI/assets/RetroGaming.ttf");
		sf::Text text(font);
		text.setString(m_helpTextStr);
		text.setCharacterSize(20);
		text.setFillColor(sf::Color::White);
		text.setPosition(sf::Vector2f(20.f, 20.f));
		
		m_helpWindow->draw(text);
		m_helpWindow->display();
	}
}

void CalculatorUI::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
	for (auto& child : children)
	{
		child->HandleEvent(event, window);
	}

	for (auto& tab : m_tabs) {
		tab->HandleEvent(event, window);
	}

	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->code == sf::Keyboard::Key::Left)
		{
			HandleButtonPress("<");
		}
		else if (keyPressed->code == sf::Keyboard::Key::Right)
		{
			HandleButtonPress(">");
		}
		else if (keyPressed->code == sf::Keyboard::Key::Escape || keyPressed->code == sf::Keyboard::Key::Delete)
		{
			HandleButtonPress("AC");
		}
		else if (keyPressed->code == sf::Keyboard::Key::Backspace)
		{
			HandleButtonPress("DEL");
		}
	}

	if (const auto* textEntered = event.getIf<sf::Event::TextEntered>())
	{
		char32_t unicode = textEntered->unicode;
		if (unicode == '\r' || unicode == '\n' || unicode == '=')
		{
			HandleButtonPress("=");
		}
		else if (unicode == '^')
		{
			HandleButtonPress("P");
		}
		else if (unicode >= 32 && unicode <= 126)
		{
			std::string label(1, static_cast<char>(unicode));
			HandleButtonPress(label);
		}
	}
}

void CalculatorUI::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	for (const auto& tab : m_tabs) {
		target.draw(*tab, states);
	}

	target.draw(m_sprite, states);
	for (const auto& child : children)
	{
		target.draw(*child, states);
	}
}

void CalculatorUI::ShowHelpWindow()
{
	std::stringstream ss;
	app::printHelp(ss, "");
	m_helpTextStr = wrapText(ss.str(), 50);

	if (!m_helpWindow || !m_helpWindow->isOpen())
	{
		m_helpWindow = std::make_unique<sf::RenderWindow>(sf::VideoMode({600, 650}), "Help - SpriteCalc");
	}
	m_helpWindow->requestFocus();
}
