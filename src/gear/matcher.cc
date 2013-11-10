//
// Copyleft 2011 RIME Developers
// License: GPLv3
//
// 2012-01-01 GONG Chen <chen.sst@gmail.com>
//
#include <boost/foreach.hpp>
#include <rime/common.h>
#include <rime/config.h>
#include <rime/schema.h>
#include <rime/segmentation.h>
#include <rime/gear/matcher.h>

namespace rime {

Matcher::Matcher(const Ticket& ticket) : Segmentor(ticket) {
  // read schema settings
  if (!ticket.schema) return;
  Config *config = ticket.schema->config();
  patterns_.LoadConfig(config);
}

bool Matcher::Proceed(Segmentation *segmentation) {
  if (patterns_.empty()) return true;
  RecognizerMatch m = patterns_.GetMatch(segmentation->input(), segmentation);
  if (m.found()) {
    DLOG(INFO) << "match: " << m.tag
               << " [" << m.start << ", " << m.end << ")";
    while (segmentation->GetCurrentStartPosition() > m.start)
      segmentation->pop_back();
    Segment segment;
    segment.start = m.start;
    segment.end = m.end;
    segment.tags.insert(m.tag);
    segmentation->AddSegment(segment);
    // terminate this round?
    //return false;
  }
  return true;
}

}  // namespace rime
