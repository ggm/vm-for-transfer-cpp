#!/bin/bash

function generate_stage1_pretransfer() {
  local TXTFILE_PATH="${1}"
  local PRETRANSFER_PATH="${2}"
  local LANG_DATA="${3}"

  cat "${TXTFILE_PATH}" |\
    apertium-destxt |\
    lt-proc "${LANG_DATA}/es-en.automorf.bin" |\
    apertium-tagger -g "${LANG_DATA}/es-en.prob" |\
    apertium-pretransfer |\
    lt-proc -b "${LANG_DATA}/es-en.autobil.bin" > ${PRETRANSFER_PATH}
}

function generate_stage1_expected() {
  local PRETRANSFER_PATH="${1}"
  local RULESDIR_PATH="${2}"
  local EXPECTED_PATH="${3}"

  cat "${PRETRANSFER_PATH}" |\
    apertium-xfervm -c "${RULESDIR_PATH}/rules.vm" > ${EXPECTED_PATH}
}

function generate_stage1_to_stage2_expected() {
  local LAST_STAGE_EXPECTED="${1}"
  local RULESDIR_PATH="${2}"
  local EXPECTED_PATH="${3}"

  cat "${LAST_STAGE_EXPECTED}" |\
    apertium-xfervm -c "${RULESDIR_PATH}/rules.vm" > ${EXPECTED_PATH}
}

function generate_stage2_to_stage3_expected() {
  local LAST_STAGE_EXPECTED="${1}"
  local RULESDIR_PATH="${2}"
  local EXPECTED_PATH="${3}"

  cat "${LAST_STAGE_EXPECTED}" |\
    apertium-xfervm -c "${RULESDIR_PATH}/rules.vm" > ${EXPECTED_PATH}
}
