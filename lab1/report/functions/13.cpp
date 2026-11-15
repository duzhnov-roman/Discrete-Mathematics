Multiset::Multiset(std::shared_ptr<const CodeSet> carrier):carrier(carrier),counts(carrier->size(), 0){}
