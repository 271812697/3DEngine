/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once


#include "OvMaths/FVector3.h"
#include "OvMaths/FTransform.h"

#include "OvAudio/Entities/AudioSource.h"

namespace OvAudio::Entities
{
	/**
	* Represents the ears of your application.
	* You can have multiple ones but only the last created will be considered by the AudioEngine
	*/
	class AudioListener
	{
	public:
		/**
		* AudioListener constructor (Internal transform management)
		*/
		AudioListener();

		/**
		* AudioListener constructor (External transform management)
		* @param p_transform
		*/
		AudioListener(OvMaths::FTransform& p_transform);

		/**
		* AudioListener destructor
		*/
		~AudioListener();

		/**
		* Returns the AudioListener FTransform
		*/
		OvMaths::FTransform& GetTransform();

		/**
		* Enable or disable the audio listener
		* @param p_enable
		*/
		void SetEnabled(bool p_enable);

		/**
		* Returns true if the audio listener is enabled
		*/
		bool IsEnabled() const;

	private:
		void Setup();

	private:
		/* Transform stuff */
		OvMaths::FTransform* const		m_transform;
		const bool						m_internalTransform;
		bool							m_enabled = true;

	public:
		static OvTools::Eventing::Event<AudioListener&>	CreatedEvent;
		static OvTools::Eventing::Event<AudioListener&>	DestroyedEvent;
	};
}