#include "textureManager.hxx"

#include "SDL2/SDL_image.h"

#include "../ThirdParty/json.hxx"


#include "basics/resources.hxx"
#include "basics/log.hxx"

void TextureManager::loadTexture(int tileID, bool colorKey)
{
  std::string fileName = Resources::getTileDataFromJSON("terrain", tileID, "filename");
  SDL_Surface *loadedImage = IMG_Load(fileName.c_str());

  if (loadedImage)
  {
    if (colorKey)
      SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0xFF, 0, 0xFF));

    _surfaceMap[tileID] = loadedImage;
    SDL_Texture *_texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), loadedImage);

    if (_texture)
      _textureMap[tileID] = _texture;
    else
      LOG(LOG_ERROR) << "Renderer could not be created! SDL Error: " << SDL_GetError();
  }
  else
    LOG(LOG_ERROR) << "Could not load Texture from file " << fileName << "\nSDL_IMAGE Error: " << IMG_GetError();
}

void TextureManager::loadTextureNew(std::string type, std::string orientation, bool colorKey)
{
  std::string fileName;


  // -----------------------
  // Read JSON File.
  // Should be read / stored during initialization

  json TileDataJSON;

  std::ifstream i("resources/data/TileDataNew.json");
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << "resources/data/TileDataNew.json"
      << " does not exist! Cannot load settings from INI File!";
    // Application should quit here, without textureData from the tileList file we can't continue
    return;
  }

  // check if json file can be parsed
  TileDataJSON = json::parse(i, nullptr, false);
  if (TileDataJSON.is_discarded())
    LOG(LOG_ERROR) << "Error parsing JSON File " << "resources/data/TileDataNew.json";

  // ------------------------
  // Parse the JSON File

  size_t idx = 0;
  while (!TileDataJSON["floor"][idx].is_null())
  {
    for (auto &it : TileDataJSON["floor"][idx].items())
    {
      if (it.key() == "orientation" && it.value() == orientation)
      {
        if (!TileDataJSON["floor"][idx]["image"].is_null())
        {
          fileName = TileDataJSON["floor"][idx]["image"].get<std::string>();
        }
      }
    }
    idx++;
  }


  SDL_Surface *loadedImage = IMG_Load(fileName.c_str());

  if (loadedImage)
  {
    if (colorKey)
      SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0xFF, 0, 0xFF));

    SDL_Texture *_texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), loadedImage);

    if (_texture)
    {
      // do stuff
    }
    else
      LOG(LOG_ERROR) << "Renderer could not be created! SDL Error: " << SDL_GetError();
  }
  else
    LOG(LOG_ERROR) << "Could not load Texture from file " << fileName << "\nSDL_IMAGE Error: " << IMG_GetError();
}

void TextureManager::loadUITexture(int uiSpriteID, bool colorKey)
{
  std::string fileName = Resources::getUISpriteDataFromJSON("button", uiSpriteID, "filename");
  std::string fileNameHover = Resources::getUISpriteDataFromJSON("button", uiSpriteID, "textureHover");
  std::string fileNamePressed = Resources::getUISpriteDataFromJSON("button", uiSpriteID, "texturePressed");
  SDL_Surface *loadedImage = IMG_Load(fileName.c_str());

  if (loadedImage)
  {
    if (colorKey)
    {
      SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0xFF, 0, 0xFF));
    }

    _uiSurfaceMap[uiSpriteID] = loadedImage;
    SDL_Texture *_texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), loadedImage);

    if (_texture != nullptr)
      _uiTextureMap[uiSpriteID] = _texture;
    else
      LOG(LOG_ERROR) << "Renderer could not be created! SDL Error: " << SDL_GetError();

    // Load hover / clicked textures if they are available.

    SDL_FreeSurface(loadedImage);

    if (!fileNameHover.empty())
    {
      SDL_Surface *loadedImage = IMG_Load(fileNameHover.c_str());
      if (loadedImage)
      {
        if (colorKey)
        {
          SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0xFF, 0, 0xFF));
        }

        SDL_Texture *_texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), loadedImage);

        if (_texture != nullptr)
          _uiTextureMapHover[uiSpriteID] = _texture;
        else
          LOG(LOG_ERROR) << "Renderer could not be created! SDL Error: " << SDL_GetError();
        SDL_FreeSurface(loadedImage);
      }
    }
    if (!fileNamePressed.empty())
    {
      SDL_Surface *loadedImage = IMG_Load(fileNamePressed.c_str());
      if (loadedImage)
      {
        if (colorKey)
        {
          SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, 0xFF, 0, 0xFF));
        }

        SDL_Texture *_texture = SDL_CreateTextureFromSurface(Resources::getRenderer(), loadedImage);

        if (_texture != nullptr)
          _uiTextureMapPressed[uiSpriteID] = _texture;
        else
          LOG(LOG_ERROR) << "Renderer could not be created! SDL Error: " << SDL_GetError();
        SDL_FreeSurface(loadedImage);
      }
    }
  }
  else
    LOG(LOG_ERROR) << "Could not load Texture from file " << fileName << "\nSDL_IMAGE Error: " << IMG_GetError();
}

SDL_Texture *TextureManager::getTileTexture(int tileID)
{
  // If the texture isn't in the map, load it first.
  if (!_textureMap.count(tileID))
  {
    loadTexture(tileID);
  }
  return _textureMap[tileID];
}

SDL_Texture *TextureManager::getUITexture(int uiSpriteID, int buttonState)
{
  if (!_uiTextureMap.count(uiSpriteID))
  {
    loadUITexture(uiSpriteID);
  }

  switch (buttonState)
  {
  case BUTTONSTATE_HOVERING:
    if (_uiTextureMapHover.count(uiSpriteID))
    {
      return _uiTextureMapHover[uiSpriteID];
    }
    else
    {
      return _uiTextureMap[uiSpriteID];
    }
  case BUTTONSTATE_CLICKED:
    if (_uiTextureMapPressed.count(uiSpriteID))
    {
      return _uiTextureMapPressed[uiSpriteID];
    }
    else
    {
      return _uiTextureMap[uiSpriteID];
    }
    return nullptr;
  default:
    return _uiTextureMap[uiSpriteID];
  }
  // If the texture isn't in the map, load it first.
}

SDL_Surface *TextureManager::getTileSurface(int tileID)
{
  // If the surface isn't in the map, load the texture first.
  if (!_textureMap.count(tileID))
  {
    loadTexture(tileID);
  }
  return _surfaceMap[tileID];
}

SDL_Surface *TextureManager::getUISurface(int uiSpriteID)
{
  // If the surface isn't in the map, load the texture first.
  if (!_textureMap.count(uiSpriteID))
  {
    loadTexture(uiSpriteID);
  }
  return _surfaceMap[uiSpriteID];
}

const SDL_Color TextureManager::getPixelColor(int tileID, int X, int Y)
{
  SDL_Color Color = {0, 0, 0, SDL_ALPHA_TRANSPARENT};

  if (_surfaceMap.find(tileID) != _surfaceMap.end())
  {
    SDL_Surface *surface = _surfaceMap[tileID];

    int Bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + Y * surface->pitch + X * Bpp;
    Uint32 pixel = *(Uint32 *)p;

    SDL_GetRGBA(pixel, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);
  }
  else
  {
    LOG(LOG_ERROR) << "No surface in map for tileID " << tileID;
  }
  return Color;
}