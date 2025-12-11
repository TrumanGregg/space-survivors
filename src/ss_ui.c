bool32 DoButton(const char* Text, int32 X, int32 Y, int32 FontSize)
{
    Rectangle ButtonBounds = {(f32)X, (f32)Y, (f32)MeasureText(Text, FontSize), (f32)FontSize};
    
    bool32 IsHovering = CheckCollisionPointRec(GetMousePosition(), ButtonBounds);
    Color TextColor = IsHovering ? YELLOW : WHITE;
    
    DrawText(Text, X, Y, FontSize, TextColor);
    
    if(IsHovering && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        PlaySound(ButtonClickSound);
        return(true);
    }
    
    return(false);
}

bool32 DoBoxedButton(const char* Text, Rectangle ButtonBounds, int32 FontSize, Color ButtonColor)
{
    bool32 IsHovering = CheckCollisionPointRec(GetMousePosition(), ButtonBounds);
    
    Color TextColor = IsHovering ? YELLOW : WHITE;
    
    int32 TextW = MeasureText(Text, FontSize);
    int32 TextX = (int32)ButtonBounds.x + ((int32)ButtonBounds.width / 2 - TextW / 2);
    int32 TextY = (int32)ButtonBounds.y + ((int32)ButtonBounds.height / 2 - FontSize / 2);
    
    DrawRectangleRec(ButtonBounds, ButtonColor);
    DrawRectangleLinesEx(ButtonBounds, 2, WHITE);
    DrawText(Text, TextX, TextY, FontSize, TextColor);
    
    if(IsHovering && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        PlaySound(ButtonClickSound);
        return(true);
    }
    
    return(false);
}