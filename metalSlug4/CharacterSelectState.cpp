#include "CharacterSelectState.h"
#include "PlayState.h"

CharacterSelectState::CharacterSelectState()
    : selectedCharacter(0),
    mLeftWasPressed(true),
    mRightWasPressed(true),
    mEnterWasPressed(true)
{

}

void CharacterSelectState::enter()
{
    selectTex.loadFromFile("Sprites/Menu/select.jpg");
    selectSprite.setTexture(selectTex);
    selectSprite.setScale(2.5f, 2.5f);
    selectSprite.setPosition(100.f, 100.f);

    mFont.loadFromFile("Sprites/Font/metal-slug.ttf");

    mTitleText.setFont(mFont);
    mTitleText.setString("SELECT PLAYERS");
    mTitleText.setCharacterSize(52);
    mTitleText.setFillColor(sf::Color::Yellow);
    mTitleText.setOutlineColor(sf::Color::Black);
    mTitleText.setOutlineThickness(3.f);
    mTitleText.setPosition(460.f, 10.f);

    mInstructionText.setFont(mFont);
    mInstructionText.setString("LEFT / RIGHT to change     ENTER to start");
    mInstructionText.setCharacterSize(20);
    mInstructionText.setFillColor(sf::Color(200, 200, 200));
    mInstructionText.setPosition(350.f, 710.f);

    const char* names[4] = { "P1", "P2", "P3", "P4" };
    float startX = 205.f;
    float spacing = 335.f;

    for (int i = 0; i < 4; i++)
    {
        mPlayerText[i].setFont(mFont);
        mPlayerText[i].setString(names[i]);
        mPlayerText[i].setOutlineColor(sf::Color::Black);
        mPlayerText[i].setOutlineThickness(2.f);
        mPlayerText[i].setPosition(startX + i * spacing, 750.f);

        if (i == selectedCharacter)
        {
            mPlayerText[i].setFillColor(sf::Color::Yellow);
            mPlayerText[i].setCharacterSize(102);
        }
        else
        {
            mPlayerText[i].setFillColor(sf::Color(200, 200, 200));
            mPlayerText[i].setCharacterSize(96);
        }
    }

    if (menuMusic.openFromFile("Sprites/Audio/audio_character_bg.ogg"))
    {
        menuMusic.setLoop(true);
        menuMusic.setVolume(30.f);
        menuMusic.play();
    }
}

void CharacterSelectState::exit()
{
    menuMusic.stop();
}

void CharacterSelectState::handleInput(sf::RenderWindow& window)
{
    bool leftNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    if (leftNow && !mLeftWasPressed)
        if (selectedCharacter > 0) selectedCharacter--;
    mLeftWasPressed = leftNow;

    bool rightNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    if (rightNow && !mRightWasPressed)
        if (selectedCharacter < 3) selectedCharacter++;
    mRightWasPressed = rightNow;

    bool enterNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
    if (enterNow && !mEnterWasPressed)
    {
        // selectedCharacter is now 0-3 (index), so pass +1 as the player count
        nextState = new PlayState(1600, 900, 0, selectedCharacter + 1);
        isDone = true;
    }
    mEnterWasPressed = enterNow;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        window.close();
}

void CharacterSelectState::update(float dt)
{
    for (int i = 0; i < 4; i++)
    {
        if (i == selectedCharacter)
        {
            mPlayerText[i].setFillColor(sf::Color::Yellow);
            mPlayerText[i].setCharacterSize(102);
        }
        else
        {
            mPlayerText[i].setFillColor(sf::Color(200, 200, 200));
            mPlayerText[i].setCharacterSize(96);
        }
    }
}

void CharacterSelectState::draw(sf::RenderWindow& window)
{
    window.draw(selectSprite);
    window.draw(mTitleText);
    window.draw(mInstructionText);

    for (int i = 0; i < 4; i++)
        window.draw(mPlayerText[i]);
}

int CharacterSelectState::getSelectedCharacter() const
{
    return selectedCharacter;
}