/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <memory>

#include <irrklang/ik_ISoundEngine.h>
#include <irrklang/ik_ISoundStopEventReceiver.h>

#include <OvTools/Eventing/Event.h>
#include <OvMaths/FVector3.h>
#include <OvMaths/FTransform.h>

#include "OvAudio/Tracking/SoundTracker.h"
#include "OvAudio/Resources/Sound.h"

namespace OvAudio::Core { class AudioPlayer; }

namespace OvAudio::Entities
{
	/**
	* Wrap Irrklang ISound
	*/
	class AudioSource
	{
	public:
		/**
		* AudioSource constructor (Internal transform management)
		* @param p_audioPlayer
		*/
		AudioSource(Core::AudioPlayer& p_audioPlayer);

		/**
		* AudioSource constructor (External transform management)
		* @param p_audioPlayer
		* @param p_transform
		*/
		AudioSource(Core::AudioPlayer& p_audioPlayer, OvMaths::FTransform& p_transform);

		/**
		* AudioSource destructor
		*/
		~AudioSource();

		/**
		* Apply the AudioSource position to the tracked sound
		*/
		void UpdateTrackedSoundPosition();

		/**
		* Apply every AudioSource settings to the currently tracked sound
		*/
		void ApplySourceSettingsToTrackedSound();

		/**
		* Returns true if a sound is currently being tracked
		*/
		bool IsTrackingSound() const;

		/**
		* Returns the currently tracked sound if any, or nullptr
		*/
		Tracking::SoundTracker* GetTrackedSound() const;

		/**
		* Defines the audio source volume
		* @param p_volume
		*/
		void SetVolume(float p_volume);

		/**
		* Defines the audio source pan (Left / Right)
		* @param p_pan
		*/
		void SetPan(float p_pan);

		/**
		* Defines if the audio source should loop
		* @param p_looped
		*/
		void SetLooped(bool p_looped);

		/**
		* Defines the audio source pitch (= frequency or playback speed)
		* @param p_pitch
		*/
		void SetPitch(float p_pitch);

		/**
		* Defines if the audio source should be spatialized or not
		* @param p_value
		*/
		void SetSpatial(bool p_value);

		/**
		* Defines the audio source attenuation threshold (Minimum distance before applying sound attenuation)
		* @param p_distance
		*/
		void SetAttenuationThreshold(float p_distance);

		/**
		* Returns the audio source volume
		*/
		float GetVolume() const;

		/**
		* Returns the audio source pan (Left / Right)
		*/
		float GetPan() const;

		/**
		* Returns if the audio source is looping
		*/
		bool IsLooped() const;

		/**
		* Returns the audio source pitch (= frequency or playback speed)
		*/
		float GetPitch() const;

		/**
		* Returns true if the audio source sound has finished
		*/
		bool IsFinished() const;

		/**
		* Returns true if the audio source is spatialized
		*/
		bool IsSpatial() const;

		/**
		* Returns the audio source attenuation threshold (Minimum distance before applying sound attenuation)
		*/
		float GetAttenuationThreshold() const;

		/**
		* Play the given sound
		* @param p_sound
		*/
		void Play(const Resources::Sound& p_sound);

		/**
		* Resume the audio source
		*/
		void Resume();

		/**
		* Pause the audio source
		*/
		void Pause();

		/**
		* Stop the audio source
		*/
		void Stop();

		/**
		* Stop the audio source and destroy the tracked sound
		*/
		void StopAndDestroyTrackedSound();

	private:
		void Setup();

	public:
		static OvTools::Eventing::Event<AudioSource&> CreatedEvent;
		static OvTools::Eventing::Event<AudioSource&> DestroyedEvent;

	private:
		Core::AudioPlayer& m_audioPlayer;

		std::unique_ptr<Tracking::SoundTracker> m_trackedSound;

		/* AudioSource settings */
		float	m_volume				= 1.0f;
		float	m_pan					= 0.0f;
		bool	m_looped				= false;
		float	m_pitch					= 1.0f;
		bool	m_spatial				= false;
		float	m_attenuationThreshold	= 1.0f;

		/* Transform stuff */
		OvMaths::FTransform* const		m_transform;
		const bool						m_internalTransform;
	};
}