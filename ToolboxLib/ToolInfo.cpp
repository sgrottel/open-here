// Open Here
// Copyright 2022 SGrottel (https://www.sgrottel.de)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//
#include "Toolbox/ToolInfo.h"

using namespace openhere::toolbox;


ToolInfo::PathRequirement ToolInfo::GetPathRequirement() const
{
	bool hasReq = false;
	bool hasReqNull = false;

	for (StartConfig const& sc : m_startConfigs)
	{
		switch (sc.pathRequirement)
		{
		case PathRequirement::DontCare:
			return PathRequirement::DontCare;
		case PathRequirement::Required:
			hasReq = true;
			break;
		case PathRequirement::RequiredNull:
			hasReqNull = true;
			break;
		}
	}

	if (hasReq && hasReqNull)
		return PathRequirement::DontCare;
	if (hasReq)
		return PathRequirement::Required;
	if (hasReqNull)
		return PathRequirement::RequiredNull;

	return PathRequirement::DontCare;
}


ToolInfo::FilesRequirement ToolInfo::GetFilesRequirement() const
{
	bool hasRequireOne = false;
	bool hasRequireOneOrMore = false;
	bool hasRequireTwoOrMore = false;
	bool hasRequireNull = false;

	for (StartConfig const& sc : m_startConfigs)
	{
		switch (sc.filesRequirement)
		{
		case FilesRequirement::DontCare:
			return FilesRequirement::DontCare;
		case FilesRequirement::RequireOne:
			hasRequireOne = true;
			break;
		case FilesRequirement::RequireOneOrMore:
			hasRequireOneOrMore = true;
			break;
		case FilesRequirement::RequireTwoOrMore:
			hasRequireTwoOrMore = true;
			break;
		case FilesRequirement::RequireNull:
			hasRequireNull = true;
			break;
		}
	}

	if (!hasRequireNull && !hasRequireOne && !hasRequireOneOrMore && !hasRequireTwoOrMore)
		return FilesRequirement::DontCare;

	if (!hasRequireNull && !hasRequireOne && !hasRequireOneOrMore && hasRequireTwoOrMore)
		return FilesRequirement::RequireTwoOrMore;

	if (!hasRequireNull && !hasRequireOne && hasRequireOneOrMore && !hasRequireTwoOrMore)
		return FilesRequirement::RequireOneOrMore;

	if (!hasRequireNull && hasRequireOne && !hasRequireOneOrMore && !hasRequireTwoOrMore)
		return FilesRequirement::RequireOne;

	if (hasRequireNull && !hasRequireOne && !hasRequireOneOrMore && !hasRequireTwoOrMore)
		return FilesRequirement::RequireNull;

	return FilesRequirement::DontCare;
}
