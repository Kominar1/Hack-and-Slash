#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
 
#include "BaseApplication.h"
 
class TutorialApplication : public BaseApplication
{
public:
  TutorialApplication();
  virtual ~TutorialApplication();
 
protected:
  virtual void createScene();
  virtual void createFrameListener();
  virtual void destroyScene();
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);
 
private:
  void defineTerrain(long x, long y);
  void initBlendMaps(Ogre::Terrain* terrain);
  void configureTerrainDefaults(Ogre::Light* light);
 
  bool mTerrainsImported;
  Ogre::TerrainGroup* mTerrainGroup;
  Ogre::TerrainGlobalOptions* mTerrainGlobals;
  CEGUI::OgreRenderer* mRenderer;
  int numKilled = 0;
  int numHealth = 100;
  CEGUI::Window* health;
  CEGUI::Window* kills1;


 
  OgreBites::Label* mInfoLabel;
 
};
 

