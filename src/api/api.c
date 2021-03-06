#include "config.h"

#include "opensubtitles.h"
#include "httpcontrol.h"
#include "api.h"
#include "tmdb.h"
#include "tvdb.h"
#include "lastfm.h"

void
api_init(void)
{
  opensub_init();
#if ENABLE_HTTPSERVER
  httpcontrol_init();
#endif
  tmdb_init();
  lastfm_init();
  tvdb_init();
}
