#include "Tutorial3.h"
 
TutorialApplication::TutorialApplication()
  : mTerrainGroup(0),
    mTerrainGlobals(0),
    mInfoLabel(0)
{
}
 
TutorialApplication::~TutorialApplication()
{
}
 
void TutorialApplication::createScene()
{

    mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
    CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
    CEGUI::Font::setDefaultResourceGroup("Fonts");
    CEGUI::Scheme::setDefaultResourceGroup("Schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
    CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
    CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
    CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
  //mCamera->setPosition(Ogre::Vector3(1913, 40, 1800));
  //mCamera->lookAt(Ogre::Vector3(1923, 50, 1660));
  mCamera->setNearClipDistance(.1);
 
  bool infiniteClip =
    mRoot->getRenderSystem()->getCapabilities()->hasCapability(
      Ogre::RSC_INFINITE_FAR_PLANE);
 
  if (infiniteClip)
    mCamera->setFarClipDistance(0);
  else
    mCamera->setFarClipDistance(50000);
 
  mSceneMgr->setAmbientLight(Ogre::ColourValue(.0, .0, .0));
 
  Ogre::Vector3 lightDir(.55, -.3, .75);
  lightDir.normalise();
 
  Ogre::Light* light = mSceneMgr->createLight("TestLight");
  light->setType(Ogre::Light::LT_DIRECTIONAL);
  light->setDirection(lightDir);
  light->setDiffuseColour(Ogre::ColourValue::White);
  light->setSpecularColour(Ogre::ColourValue(.4, .4, .4));
  
 
  // Fog
 Ogre::ColourValue fadeColour(.5, .5, .5);
  mWindow->getViewport(0)->setBackgroundColour(fadeColour);
 
mSceneMgr->setFog(Ogre::FOG_EXP2, fadeColour, 0.002);
 
  // Terrain
  mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
 
  mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(
    mSceneMgr,
    Ogre::Terrain::ALIGN_X_Z,
    513, 12000.0);
  mTerrainGroup->setFilenameConvention(Ogre::String("terrain"), Ogre::String("dat"));
  mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
 
  configureTerrainDefaults(light);
 
  for (long x = 0; x <= 0; ++x)
    for (long y = 0; y <= 0; ++y)
      defineTerrain(x, y);
 
  mTerrainGroup->loadAllTerrains(true);
 
  if (mTerrainsImported)
  {
    Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
 
    while (ti.hasMoreElements())
    {
      Ogre::Terrain* t = ti.getNext()->instance;
      initBlendMaps(t);
    }
  }
 
  mTerrainGroup->freeTemporaryResources();
 
  // Sky Techniques
  // mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 300, false);
  mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
  // Ogre::Plane plane;
  // plane.d = 1000;
  // plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
 
  // mSceneMgr->setSkyPlane(
  //   true, plane, "Examples/SpaceSkyPlane", 1500, 40, true, 1.5, 150, 150);

  ninjaEntity = mSceneMgr->createEntity("ninja.mesh");
  ninjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
  ninjaNode->attachObject(ninjaEntity);
  ninjaEntity->setCastShadows(true);
  ninjaNode->setPosition(Ogre::Vector3(1950, 50, 1660));
  ninjaNode->setScale(0.3,0.3,0.3);
  ninjaNode->yaw(Ogre::Degree(180));

  cameraNode = ninjaNode->createChildSceneNode();
  cameraNode->attachObject(mCamera);
  cameraNode->setPosition(Ogre::Vector3(0, 150, 0));

  mAnimationState = ninjaEntity->getAnimationState("Idle1");
  mAnimationState->setLoop(true);
  mAnimationState->setEnabled(true);

  CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
  CEGUI::Window* sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

  CEGUI::Window* kills = wmgr.createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
  kills->setText("Kills");
  kills->setSize(CEGUI::USize(CEGUI::UDim(0.10, 0), CEGUI::UDim(0.05, 0)));
  kills->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 50)));
  sheet->addChild(kills);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);

  kills1 = wmgr.createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
  kills1->setText(std::to_string(numKilled));
  kills1->setSize(CEGUI::USize(CEGUI::UDim(0.10, 0), CEGUI::UDim(0.05, 0)));
  kills1->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 110), CEGUI::UDim(0, 50)));
  sheet->addChild(kills1);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);

  
  health = wmgr.createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
  health->setText(std::to_string(numHealth));
  health->setSize(CEGUI::USize(CEGUI::UDim(0.10, 0), CEGUI::UDim(0.05, 0)));
  health->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 110), CEGUI::UDim(0, 0)));
  sheet->addChild(health);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);

  CEGUI::Window* health1 = wmgr.createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
  health1->setText("Health");
  health1->setSize(CEGUI::USize(CEGUI::UDim(0.10, 0), CEGUI::UDim(0.05, 0)));
  sheet->addChild(health1);
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);
  /*
  Ogre::Entity* knotEntity = mSceneMgr->createEntity("knot.mesh");
  knotNode = ninjaNode->createChildSceneNode();
  knotNode->attachObject(knotEntity);
  knotNode->setPosition(Ogre::Vector3(0, 100, 0));
  knotNode->setScale(0.01, 0.01, 0.01);

  Ogre::Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");
  ogreNode = knotNode->createChildSceneNode();
  ogreNode->attachObject(ogreEntity);
  ogreNode->setScale(75, 75, 75);
  ogreNode->setPosition(Ogre::Vector3(0, 0, -15000));

  Ogre::Light* spotLight = mSceneMgr->createLight("SpotLight");
  spotLight->setDiffuseColour(1, 1, 1);
  spotLight->setSpecularColour(1, 1, 1);
  spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
  spotLight->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Z);
  Ogre::SceneNode* spotLightNode = ogreNode->createChildSceneNode();
  spotLightNode->attachObject(spotLight);
  spotLightNode->setDirection(0, 0, 0);
  spotLightNode->setPosition(Ogre::Vector3(0, 0, 30));
  spotLight->setSpotlightRange(Ogre::Degree(500), Ogre::Degree(500));

  Ogre::Light* spotLight2 = mSceneMgr->createLight("SpotLight2");
  spotLight2->setDiffuseColour(1, 1, 1);
  spotLight2->setSpecularColour(1, 1, 1);
  spotLight2->setType(Ogre::Light::LT_SPOTLIGHT);
  spotLight2->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Z);
  Ogre::SceneNode* spotLightNode2 = ninjaNode->createChildSceneNode();
  spotLightNode2->attachObject(spotLight2);
  spotLightNode2->setDirection(0, 0, 0);
  spotLightNode2->setPosition(Ogre::Vector3(0, 0, 30));
  spotLight2->setSpotlightRange(Ogre::Degree(500), Ogre::Degree(500));
  */
}
 
void TutorialApplication::createFrameListener()
{
  BaseApplication::createFrameListener();
 
  mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TerrainInfo", "", 350);
}
 
void TutorialApplication::destroyScene()
{
  OGRE_DELETE mTerrainGroup;
  OGRE_DELETE mTerrainGlobals;
}
 
bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
  bool ret = BaseApplication::frameRenderingQueued(fe);
 
  if (mTerrainGroup->isDerivedDataUpdateInProgress())
  {
    mTrayMgr->moveWidgetToTray(mInfoLabel, OgreBites::TL_TOP, 0);
    mInfoLabel->show();
 
    if (mTerrainsImported)
      mInfoLabel->setCaption("Building terrain...");
    else
      mInfoLabel->setCaption("Updating textures...");
  }
  else
  {
    mTrayMgr->removeWidgetFromTray(mInfoLabel);
    mInfoLabel->hide();
 
    if (mTerrainsImported)
    {
      mTerrainGroup->saveAllTerrains(true);
      mTerrainsImported = false;
    }
  }

  ninjaNode->setPosition(ninjaNode->getPosition().x, mTerrainGroup->getHeightAtWorldPosition(ninjaNode->getPosition(), NULL), ninjaNode->getPosition().z);

  return ret;
}
 
void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
  img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
 
  if (flipX)
    img.flipAroundY();
  if (flipY)
    img.flipAroundX();
 
}
 
void TutorialApplication::defineTerrain(long x, long y)
{
  Ogre::String filename = mTerrainGroup->generateFilename(x, y);
 
  bool exists =
    Ogre::ResourceGroupManager::getSingleton().resourceExists(
      mTerrainGroup->getResourceGroup(),
      filename);
 
  if (exists)
    mTerrainGroup->defineTerrain(x, y);
  else
  {
    Ogre::Image img;
    getTerrainImage(x % 2 != 0, y % 2 != 0, img);
    mTerrainGroup->defineTerrain(x, y, &img);
 
    mTerrainsImported = true;
  }
}
 
void TutorialApplication::initBlendMaps(Ogre::Terrain* terrain)
{
  Ogre::Real minHeight0 = 70;
  Ogre::Real fadeDist0 = 40;
  Ogre::Real minHeight1 = 70;
  Ogre::Real fadeDist1 = 15;
 
  Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
  Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
 
  float* pBlend0 = blendMap0->getBlendPointer();
  float* pBlend1 = blendMap1->getBlendPointer();
 
  for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
  {
    for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
    {
      Ogre::Real tx, ty;
 
      blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
      Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
      Ogre::Real val = (height - minHeight0) / fadeDist0;
      val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
      *pBlend0++ = val;
 
      val = (height - minHeight1) / fadeDist1;
      val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
      *pBlend1++ = val;
    }
  }
 
  blendMap0->dirty();
  blendMap1->dirty();
  blendMap0->update();
  blendMap1->update();
 
}
 
void TutorialApplication::configureTerrainDefaults(Ogre::Light* light)
{
  mTerrainGlobals->setMaxPixelError(8);
  mTerrainGlobals->setCompositeMapDistance(3000);
 
  mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
  mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
  mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
 
  Ogre::Terrain::ImportData& importData = mTerrainGroup->getDefaultImportSettings();
  importData.terrainSize = 513;
  importData.worldSize = 12000.0;
  importData.inputScale = 600;
  importData.minBatchSize = 33;
  importData.maxBatchSize = 65;
 
  importData.layerList.resize(3);
  importData.layerList[0].worldSize = 100;
  importData.layerList[0].textureNames.push_back(
    "dirt_grayrocky_diffusespecular.dds");
  importData.layerList[0].textureNames.push_back(
    "dirt_grayrocky_normalheight.dds");
  importData.layerList[1].worldSize = 30;
  importData.layerList[1].textureNames.push_back(
    "grass_green-01_diffusespecular.dds");
  importData.layerList[1].textureNames.push_back(
    "grass_green-01_normalheight.dds");
  importData.layerList[2].worldSize = 200;
  importData.layerList[2].textureNames.push_back(
    "growth_weirdfungus-03_diffusespecular.dds");
  importData.layerList[2].textureNames.push_back(
    "growth_weirdfungus-03_normalheight.dds");
 
}
 
 
#if Ogre_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
  int main(int argc, char *argv[])
#endif
  {
    // Create application object
    TutorialApplication app;
 
    try {
      app.go();
    } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      MessageBoxA( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
      std::cerr << "An exception has occured: " <<
	e.getFullDescription().c_str() << std::endl;
#endif
    }
 
    return 0;
  }
 
#ifdef __cplusplus
}
#endif