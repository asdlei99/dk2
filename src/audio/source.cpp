#include "audio/debug.h"
#include "audio/source.h"

namespace dk::audio
{

source::source() noexcept
	: m_id(0)
{}

source::~source() noexcept
{
	if (m_id != 0)
		AL_CALL(alDeleteSources(1, &m_id));
}

void source::play() const noexcept
{
	AL_CALL(alSourcePlay(m_id));
}

void source::pause() const noexcept
{
	AL_CALL(alSourcePause(m_id));
}

void source::stop() const noexcept
{
	AL_CALL(alSourceStop(m_id));
}

void source::set_sound(const sound& snd) const noexcept
{
	AL_CALL(alSourcei(m_id, AL_BUFFER, snd.id()));
}

void source::remove_sound() const noexcept
{
	this->stop();
	AL_CALL(alSourcei(m_id, AL_BUFFER, 0));
}

void source::set_pitch(float val) const noexcept
{
	AL_CALL(alSourcef(m_id, AL_PITCH, val));
}

void source::set_gain(float val) const noexcept
{
	AL_CALL(alSourcef(m_id, AL_GAIN, val));
}

void source::set_pos(float x, float y, float z) const noexcept
{
	AL_CALL(alSource3f(m_id, AL_POSITION, x, y, z));
}

void source::set_dir(float x, float y, float z) const noexcept
{
	AL_CALL(alSource3f(m_id, AL_DIRECTION, x, y, z));
}

void source::set_velocity(float x, float y, float z) const noexcept
{
	AL_CALL(alSource3f(m_id, AL_VELOCITY, x, y, z));
}

void source::set_looping(bool val) const noexcept
{
	AL_CALL(alSourcei(m_id, AL_LOOPING, val));
}

float source::pitch() const noexcept
{
	float val;
	AL_CALL(alGetSourcef(m_id, AL_PITCH, &val));
	return val;
}

float source::gain() const noexcept
{
	float val;
	AL_CALL(alGetSourcef(m_id, AL_GAIN, &val));
	return val;
}

math::vec3f source::pos() const noexcept
{
	math::vec3f val;
	AL_CALL(alGetSource3f(m_id, AL_POSITION, &val.x, &val.y, &val.z));
	return val;
}

math::vec3f source::dir() const noexcept
{
	math::vec3f val;
	AL_CALL(alGetSource3f(m_id, AL_DIRECTION, &val.x, &val.y, &val.z));
	return val;
}

math::vec3f source::velocity() const noexcept
{
	math::vec3f val;
	AL_CALL(alGetSource3f(m_id, AL_VELOCITY, &val.x, &val.y, &val.z));
	return val;
}

bool source::looping() const noexcept
{
	int val;
	AL_CALL(alGetSourcei(m_id, AL_LOOPING, &val));
	return val;
}

status source::create() noexcept
{
	AL_CALL(alGenSources(1, &m_id));
	this->set_pitch(1.0f);
	this->set_gain(1.0f);
	this->set_pos(0.0f, 0.0f, 0.0f);
	this->set_velocity(0.0f, 0.0f, 0.0f);
	this->set_looping(false);

	return status::OK;
}

}