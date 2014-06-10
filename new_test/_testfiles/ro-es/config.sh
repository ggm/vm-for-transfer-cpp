#!/bin/bash

function generate_stage1_pretransfer() {
  local TXTFILE_PATH="${1}"
  local PRETRANSFER_PATH="${2}"
  local LANG_DATA="${3}"

  cat "${TXTFILE_PATH}" |\
    apertium-destxt |\
    lt-proc "${LANG_DATA}/ro-es.automorf.bin" |\
    apertium-tagger -g "${LANG_DATA}/ro-es.prob" |\
    apertium-pretransfer |\
    lt-proc -b "${LANG_DATA}/ro-es.autobil.bin" > ${PRETRANSFER_PATH}
}

function generate_stage1_expected() {
  local PRETRANSFER_PATH="${1}"
  local RULESDIR_PATH="${2}"
  local EXPECTED_PATH="${3}"

  cat "${PRETRANSFER_PATH}" |\
    apertium-xfervm -c "${RULESDIR_PATH}/rules.vm" > ${EXPECTED_PATH}
}

