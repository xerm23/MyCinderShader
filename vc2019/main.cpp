#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

using namespace ci;
using namespace ci::app;




class BasicApp : public App {
public:
	void	setup() override;
	void	draw() override;

	CameraPersp         mCam;
	gl::BatchRef        mRect;
	gl::GlslProgRef		mGlsl;
	gl::TextureRef		mTex;
	gl::TextureRef		mTex1;
	int totalTextures = 0;

	void fileDrop(FileDropEvent event) override;
	std::vector<gl::TextureRef> textures;

	void createShader();

};

/// To load images via drop...
void BasicApp::fileDrop(FileDropEvent event) {
	auto img = loadImage(event.getFile(0));
	if (totalTextures == 0) {
		mTex = gl::Texture2d::create(img);
		mTex->bind(0);

		mTex1 = gl::Texture2d::create(img);
		mTex1->bind(1);
	}
	else {
		mTex = mTex1;
		mTex->bind(0);
		mTex1 = gl::Texture2d::create(img);
		mTex1->bind(1);
	}
	totalTextures++;
	createShader();

}


void BasicApp::createShader() {
	//vertex shader
	mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
		.vertex(CI_GLSL(150,
			uniform mat4	ciModelViewProjection;
	in vec4			ciPosition;
	in vec2			ciTexCoord0;
	out vec2		TexCoord;

	void main(void) {
		gl_Position = ciModelViewProjection * ciPosition;
		TexCoord = ciTexCoord0;
	}
	))
		//fragment shader
		.fragment(CI_GLSL(150,
			out vec4 FragColor;

	in vec4 ourColor;
	in vec2 TexCoord;

	uniform sampler2D ourTexture1;
	uniform sampler2D ourTexture2;

	void main() {
		FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), step(0.5, TexCoord.x));
	}
	)));

	mGlsl->uniform("ourTexture1", 0);
	mGlsl->uniform("ourTexture2", 1);

	mRect = gl::Batch::create(geom::Rect(), mGlsl);


}


void BasicApp::setup()
{
	mCam.lookAt(vec3(0, 0, -2), vec3(0));
	mCam.setWorldUp(vec3(0, -1, 0));

	setWindowSize(1200, 700);
	setWindowPos(100, 70);

	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void BasicApp::draw()
{
	gl::clear(Color(0.2f, 0.2f, 0.2f));
	gl::setMatrices(mCam);

	if (mRect != NULL)
		mRect->draw();


	gl::setMatricesWindow(getWindowSize(), true);
	gl::draw(mTex,
		Rectf(0, getWindowHeight() - 100,
			100 * getWindowWidth() / getWindowHeight(), getWindowHeight()));
	gl::draw(mTex1,
		Rectf(getWindowWidth() - 100 * getWindowWidth() / getWindowHeight(), getWindowHeight() - 100,
			getWindowWidth(), getWindowHeight()));
}

CINDER_APP(BasicApp, RendererGl);




