//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2012-04-22 GONG Chen <chen.sst@gmail.com>
//
#ifndef RIME_TRANSLATOR_COMMONS_H_
#define RIME_TRANSLATOR_COMMONS_H_

#include <set>
#include <string>
#include <vector>
#include <boost/regex.hpp>
#include <rime/common.h>
#include <rime/config.h>
#include <rime/candidate.h>
#include <rime/translation.h>
#include <rime/algo/algebra.h>
#include <rime/algo/syllabifier.h>
#include <rime/dict/vocabulary.h>

namespace rime {

//

class Patterns : public std::vector<boost::regex> {
 public:
  bool Load(ConfigListPtr patterns);
};

//

class Spans {
 public:
  void AddVertex(size_t vertex);
  void AddSpan(size_t start, size_t end);
  void AddSpans(const Spans& spans);
  void Clear();
  // move by syllable by returning a value different from caret_pos
  size_t PreviousStop(size_t caret_pos) const;
  size_t NextStop(size_t caret_pos) const;
  size_t Count(size_t start_pos, size_t end_pos) const;
  size_t start() const {
    return vertices_.empty() ? 0 : vertices_.front();
  }
  size_t end() const {
    return vertices_.empty() ? 0 : vertices_.back();
  }
  void set_vertices(std::vector<size_t>&& vertices) {
    vertices_ = vertices;
  }

 private:
  std::vector<size_t> vertices_;
};

class Phrase;

class PhraseSyllabifier {
 public:
  virtual ~PhraseSyllabifier() = default;

  virtual Spans Syllabify(const Phrase* phrase) = 0;
};

//

class Language;

class Phrase : public Candidate {
 public:
  Phrase(Language* language,
         const std::string& type, size_t start, size_t end,
         const shared_ptr<DictEntry>& entry)
      : Candidate(type, start, end),
        language_(language),
        entry_(entry) {
  }
  const std::string& text() const { return entry_->text; }
  std::string comment() const { return entry_->comment; }
  std::string preedit() const { return entry_->preedit; }
  void set_comment(const std::string& comment) {
    entry_->comment = comment;
  }
  void set_preedit(const std::string& preedit) {
    entry_->preedit = preedit;
  }
  void set_syllabifier(shared_ptr<PhraseSyllabifier> syllabifier) {
    syllabifier_ = syllabifier;
  }

  double weight() const { return entry_->weight; }
  Code& code() const { return entry_->code; }
  const DictEntry& entry() const { return *entry_; }
  Language* language() const { return language_; }
  Spans spans() {
    return syllabifier_ ? syllabifier_->Syllabify(this)
                        : Spans();
  }

 protected:
  Language* language_;
  shared_ptr<DictEntry> entry_;
  shared_ptr<PhraseSyllabifier> syllabifier_;
};

//

class Sentence : public Phrase {
 public:
  Sentence(Language* language)
      : Phrase(language, "sentence", 0, 0, New<DictEntry>()) {
    entry_->weight = 1.0;
  }
  Sentence(const Sentence& other)
      : Phrase(other),
        components_(other.components_),
        syllable_lengths_(other.syllable_lengths_) {
    entry_ = New<DictEntry>(other.entry());
  }
  void Extend(const DictEntry& entry, size_t end_pos);
  void Offset(size_t offset);

  const std::vector<DictEntry>& components() const {
    return components_;
  }
  const std::vector<size_t>& syllable_lengths() const {
    return syllable_lengths_;
  }

 protected:
  std::vector<DictEntry> components_;
  std::vector<size_t> syllable_lengths_;
};

//

struct Ticket;

class TranslatorOptions {
 public:
  TranslatorOptions(const Ticket& ticket);
  bool IsUserDictDisabledFor(const std::string& input) const;

  const std::string& delimiters() const { return delimiters_; }
  const std::string& tag() const { return tag_; }
  void set_tag(const std::string& tag) { tag_ = tag; }
  bool enable_completion() const { return enable_completion_; }
  void set_enable_completion(bool enabled) { enable_completion_ = enabled; }
  bool strict_spelling() const { return strict_spelling_; }
  void set_strict_spelling(bool is_strict) { strict_spelling_ = is_strict; }
  double initial_quality() const { return initial_quality_; }
  void set_initial_quality(double quality) { initial_quality_ = quality; }
  Projection& preedit_formatter() { return preedit_formatter_; }
  Projection& comment_formatter() { return comment_formatter_; }

 protected:
  std::string delimiters_;
  std::string tag_ = "abc";
  bool enable_completion_ = true;
  bool strict_spelling_ = false;
  double initial_quality_ = 0.;
  Projection preedit_formatter_;
  Projection comment_formatter_;
  Patterns user_dict_disabling_patterns_;
};

}  // namespace rime

#endif  // RIME_TRANSLATOR_COMMONS_H_
