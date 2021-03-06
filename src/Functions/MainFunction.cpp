//
//  MainFunction.cpp
//  BassLive 2.0
//
//  Created by Bass Jansson on 18/02/16.
//
//

#include "Syntax.h"


//========================================================================
MainFunction::MainFunction (Memory* memory) : Function(CHAR_FUNC_MAIN_OPEN,
                                                       CHAR_FUNC_MAIN_CLOSE)
{
    charType = MAIN;
    typeType = MODULE;
    
    
    charFont.load("fonts/Menlo-Bold.ttf", FONT_SIZE);
    charWidth    = charFont.stringWidth("X");
    charHeight   = charFont.stringHeight("Xgj{|");
    charSelected = begin;
    
    
    this->memory = memory;
    this->cursorTime = 0;
    this->cursorAnim = 0.0f;
    
    
    a[0] =  1.0f;
    a[1] = -0.8f;
    
    for (int i = 0; i < NEWTON_SIZE; ++i)
    {
        coefs[i][0] = ofRandom(-0.02f, 0.02f);
        coefs[i][1] = ofRandom(-0.02f, 0.02f);
    }
    
    
                  add(new Character('B'));
    charSelected->add(new Character('a'));
    charSelected->add(new Character('s'));
    charSelected->add(new Character('s'));
    charSelected->add(new Character('L'));
    charSelected->add(new Character('i'));
    charSelected->add(new Character('v'));
    charSelected->add(new Character('e'));
    
    
    flash(COLOR_FUNC_MAIN);
}

MainFunction::~MainFunction()
{
    remove(true);
}

//========================================================================
void MainFunction::draw()
{
    // Draw main function
    float x = 0.0f;
    float y = charHeight;
    Function::draw(x, y, HORIZONTAL, false, false);
    
    
    // Draw cursor and update cursor time
    charSelected->drawCursor(cursorAnim);
    
    cursorTime = (cursorTime + 1) % FRAME_RATE;
    
    if (cursorTime < FRAME_RATE / 2)
        cursorAnim += 0.9f * (1.0f - cursorAnim);
    else
        cursorAnim -= 0.3f * cursorAnim;
    
    
    // Update RMS and newton fractal coefficients
    RMS = 0.9f * RMS + 0.1f * (memory->getDAC()->getRMS().L +
                               memory->getDAC()->getRMS().R) * 0.5f;
    
    a[0] =  0.75f + RMS;
    a[1] = -0.50f - RMS;
    coefs[0][0] = sinf(ofGetSystemTime() / 130000.0f * TWO_PI * (RMS * 0.1f + 1.0f)) * coefs[1][0] * 2.0f;
    coefs[0][1] = cosf(ofGetSystemTime() / 170000.0f * TWO_PI * (RMS * 0.1f + 1.0f)) * coefs[1][1] * 2.0f;
        
}

//========================================================================
void MainFunction::keyPressed (int key)
{
    // Reset cursor time when typing
    cursorTime = 0;
    
    
    // Add and remove characters
    if (charSelected != end())
    {
        if (ofGetKeyPressed(OF_KEY_COMMAND))
        {
            switch (key)
            {
                case 'i': add(new InputType());      break;
                case 'm': add(new ModuleType());     break;
                case 'n': add(new ModuleFunction()); break;
                    
                case 'r':
                case OF_KEY_RETURN:
                    if (ofGetKeyPressed(OF_KEY_SHIFT))
                    {
                        // Change newton fractal coefficients randomly
                        int a = int(ofRandom(NEWTON_SIZE - 1)) + 1;
                        int b = int(ofRandom(2));
                        if (fabsf(coefs[a][b]) > 0.8f)
                            coefs[a][b] *= -0.1f;
                        else if (coefs[a][b] < 0.0f)
                            coefs[a][b] -= 0.02f + ofRandom(0.2f);
                        else
                            coefs[a][b] += 0.02f + ofRandom(0.2f);
                           
                        // Compile everything
                        charSelected = begin;
                    }
                    charSelected->getParentType()->compile(memory, key == 'r');
                    break;
            }
        }
        else
        {
            Type* parent = charSelected->getParentType();
            if (parent == this)
                Function::keyPressed(key);
            else
                parent->keyPressed(key);
            
            switch (key)
            {
                case CHAR_TYPE_NUMBER : charSelected->add(new  NumberType()); break;
                case CHAR_TYPE_PERCENT: charSelected->add(new PercentType()); break;
                case CHAR_TYPE_INPUT  : charSelected->add(new   InputType()); break;
                case CHAR_TYPE_MOD_ID : charSelected->add(new  ModuleType()); break;
                case CHAR_TYPE_BUF_ID : charSelected->add(new  BufferType()); break;
                    
                case CHAR_FUNC_MOD_OPEN: charSelected->add(new ModuleFunction()); break;
                case CHAR_FUNC_BUF_OPEN: charSelected->add(new BufferFunction()); break;
                    
                case ' ':
                    if (ofGetKeyPressed(OF_KEY_SHIFT))
                        charSelected->add(new PercentType());
                    else
                        charSelected->add(new NumberType());
                    break;
                    
                case OF_KEY_RETURN:
                    if (ofGetKeyPressed(OF_KEY_SHIFT))
                        charSelected->add(new BufferFunction());
                    else
                        charSelected->add(new ModuleFunction());
                    break;
                    
                case OF_KEY_BACKSPACE:
                    if (ofGetKeyPressed(OF_KEY_SHIFT))
                        charSelected = charSelected->getParentType();
                    charSelected->remove();
                    break;
            }
        }
    }
    
    
    // Move cursor
    if (ofGetKeyPressed(OF_KEY_SHIFT))
    {
        switch (key)
        {
            case OF_KEY_TAB:
                charSelected = charSelected->getType(LEFT);
                break;
                
            case OF_KEY_LEFT:
                charSelected = charSelected->getType(LEFT);
                break;
                
            case OF_KEY_RIGHT:
                charSelected = charSelected->getType(RIGHT);
                break;
                
            case OF_KEY_UP:
                charSelected = charSelected->getParentType();
                break;
                
            case OF_KEY_DOWN:
                charSelected = charSelected->end();
                break;
        }
    }
    else
    {
        switch (key)
        {
            case OF_KEY_TAB:
                charSelected = charSelected->getType(RIGHT);
                break;
                
            case OF_KEY_LEFT:
                charSelected = charSelected->left;
                break;
                
            case OF_KEY_RIGHT:
                charSelected = charSelected->right;
                break;
                
            case OF_KEY_UP:
                charSelected = charSelected->getFunction(UP);
                break;
                
            case OF_KEY_DOWN:
                charSelected = charSelected->getFunction(DOWN);
                break;
        }
    }
}

sig* MainFunction::compile (Memory* memory, bool record)
{
    sig_vec inputs;
    
    for (Character* c = getType(RIGHT);
         c != end();
         c = c->end()->right)
    {
        Type* t = (Type*)c;
        
        inputs.push_back(t->compile(memory, record));
    }
    
    memory->getDAC()->setInputs(inputs);
    inputs.clear();
    
    flash(COLOR_FUNC_MAIN);
}

//========================================================================
void MainFunction::mousePressed (float x, float y, int button)
{
    if (button == OF_MOUSE_BUTTON_LEFT || button == OF_MOUSE_BUTTON_MIDDLE)
    {
        for (Character* c = begin; c != end(); c = c->right)
        {
            if ((x >= c->x && x < c->x + charWidth) &&
                (y >= c->y && y < c->y + charHeight))
            {
                if (button == OF_MOUSE_BUTTON_LEFT)
                {
                    charSelected = c;
                }
                else
                {
                    if (c->charType == TYPE)
                        ((Type*)c)->copyTo(begin);
                    else if (c->charType == FUNC)
                        ((Function*)c)->copyTo(begin);
                }
                
                return;
            }
        }
    }
}

void MainFunction::mouseReleased (float x, float y, int button)
{
    x -= charWidth * 0.5f;
    
    if (button == OF_MOUSE_BUTTON_LEFT && charSelected->charType != CHAR)
    {
        for (Character* c = begin; c != end(); c = c->right)
        {
            if (c != charSelected &&
                (x >= c->x && x < c->x + charWidth) &&
                (y >= c->y && y < c->y + charHeight))
            {
                c->add(charSelected, false);
                return;
            }
        }
    }
}
