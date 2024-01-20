#include "scene.h"

namespace demo
{
	scene::scene(physics::world& world)
		: world(world)
	{}

	bool scene::allows_zoom() const
	{
		return allow_zoom;
	}

	bool scene::allows_pan() const
	{
		return allow_pan;
	}

	bool scene::updates_on_pause() const
	{
		return update_on_pause;
	}

	demo::scene_default_settings scene::get_default_settings() const
	{
		return scene_defaults;
	}
}