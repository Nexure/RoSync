#pragma once

enum patch_type {
	FILE_ADDED,

};

class file_patch {
private:
	const patch_type patch;

public:
	file_patch(patch_type patch);
	~file_patch();

	const patch_type& get_type() const;
};