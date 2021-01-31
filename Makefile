# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

################################################################################
# What Docker images shall be used?
REPOSITORY=chrberger
ARCH=amd64
IMAGE=ubuntu-$(ARCH)
BASE_IMAGE=$(REPOSITORY)/$(IMAGE)
BASE_IMAGE_VERSION=latest
BASE_IMAGE_DEV_SUFFIX=-dev
BASE_IMAGE_DEV=$(REPOSITORY)/$(IMAGE)$(BASE_IMAGE_DEV_SUFFIX)
BASE_IMAGE_DEV_VERSION=$(BASE_IMAGE_VERSION)

# Use Alpine to build:
# make ARCH=amd64 IMAGE=alpine-amd64 DOCKERFILE_FOR_BUILDING=Dockerfile.Alpine-3.7

################################################################################
# Product names.
PRODUCT=libcluon-on-
SOURCE_FOLDER=libcluon

################################################################################
# Tools.
DOCKER=$(shell which docker)
CMAKE=$(shell which cmake)

################################################################################
# Directories.
PATH_CCACHE=$(HOME)/.ccache
PATH_SOURCES_FOLDER=$(shell pwd)
PATH_BUILDTOOLS_FOLDER=$(PATH_SOURCES_FOLDER)/buildtools/docker
PATH_BUILDS_FOLDER=$(PATH_SOURCES_FOLDER)/builds
PATH_FOR_BUILDING=$(PATH_BUILDS_FOLDER)/$(PRODUCT)$(shell echo $(BASE_IMAGE) | cut -f2 -d"/")-$(GIT_CURRENT_BRANCH)
PATH_FOR_INSTALL=$(PATH_BUILDS_FOLDER)/$(PRODUCT)$(shell echo $(BASE_IMAGE) | cut -f2 -d"/")-$(GIT_CURRENT_BRANCH)/opt
DOCKERFILE_FOR_BUILDING=$(PATH_BUILDTOOLS_FOLDER)/Dockerfile.Ubuntu-18.04

################################################################################
# Determine Git-related setting.
GIT_CURRENT_BRANCH=$(shell git rev-parse --abbrev-ref HEAD)

################################################################################
define runOnCode
	$(CMAKE) -E make_directory $(PATH_FOR_BUILDING)
	$(CMAKE) -E make_directory $(PATH_FOR_INSTALL)
	$(DOCKER) run --rm -ti -v /etc/group:/etc/group:ro \
			--net=host \
			--cap-add SYS_PTRACE \
			-e USER_FOR_BUILDING=$$USER \
			-v /etc/passwd:/etc/passwd:ro \
			-v /etc/shadow:/etc/shadow:ro \
			-v $(PATH_SOURCES_FOLDER):/opt/sources \
			-v $(PATH_BUILDTOOLS_FOLDER)/build.sh:/opt/build.sh:ro \
			-v $(PATH_FOR_BUILDING):/opt/build \
			-v $(PATH_FOR_INSTALL):/opt/install \
			-v $(PATH_CCACHE):/opt/ccache \
			-w /opt/build \
			$(BASE_IMAGE_DEV):$(BASE_IMAGE_DEV_VERSION) \
			/opt/build.sh $1 ./$(SOURCE_FOLDER)/
endef

.PHONY: all clean createDockerBuildImage build compile test install memory-leaks code-coverage show-coverage reformat-code static-code-analysis performance deploy deployToAlpine deployToAlpine-armhf deployToAlpine-aarch64 deployToJavaScript deployToLaunchpad deployToHeaderOnly docs

################################################################################
all: build

clean:
	$(CMAKE) -E remove_directory $(PATH_FOR_BUILDING)

createDockerBuildImage:
	cd $(PATH_BUILDTOOLS_FOLDER) && $(DOCKER) build --no-cache -t $(BASE_IMAGE_DEV) -f $(DOCKERFILE_FOR_BUILDING) .

build: compile test install

compile:
	$(call runOnCode,compile)

test:
	$(call runOnCode,test)

install:
	$(call runOnCode,install)

memory-leaks:
	$(call runOnCode,memory-leaks)

code-coverage:
	$(call runOnCode,code-coverage)

show-coverage:
	$(call runOnCode,show-coverage)

reformat-code:
	$(call runOnCode,reformat-code)

static-code-analysis:
	$(call runOnCode,static-code-analysis)

performance:
	$(call runOnCode,performance)

deployToAlpine:
	./buildtools/deploy/deployToAlpine.sh

deployToJavaScript:
	./buildtools/deploy/deployToJavaScript.sh

deployToLaunchpad:
	./buildtools/deploy/deployToLaunchpad.sh

deployToHeaderOnly:
	./buildtools/deploy/deployToHeaderOnly.sh

deploy: deployToLaunchpad docs deployToHeaderOnly deployToJavaScript deployToAlpine

docs:
	./buildtools/deploy/generateDocumentation.sh

