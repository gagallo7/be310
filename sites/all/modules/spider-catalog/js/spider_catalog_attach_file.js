var i = 0;
/**
 * Delete uploaded file.
 */
function spider_catalog_delete_file(type, j) {
  var uploadsDiv = document.getElementById("viewUploadsDiv");
  switch (type) {
    case "img":
      var removeImageLoader = document.getElementById("imguploaded_id_" + j);
      uploadsDiv.removeChild(removeImageLoader);
      break;

    case "img_brows":
      var input_file_container = document.getElementById("input_file_container");
      var remove_img_brows = document.getElementById(j);
      input_file_container.removeChild(remove_img_brows);
      break;
  }
}

/**
 * Add image upload field.
 */
function spider_catalog_add_upload_box() {
  var input_file_container = document.getElementById("input_file_container");
  // Image upload div.
  var inputDiv = document.createElement("div");
  inputDiv.setAttribute("id", "edit-image-upload-wrapper_" + i + "");
  inputDiv.setAttribute("class", "form-item");
  input_file_container.appendChild(inputDiv);
  // Image upload input.
  var inputBox = document.createElement("input");
  inputBox.setAttribute("id", "edit-image-upload");
  inputBox.setAttribute("name", "files[image_upload_" + i + "]");
  inputBox.setAttribute("class", "form-file");
  inputBox.setAttribute("type", "file");
  inputBox.setAttribute("size", "60");
  inputDiv.appendChild(inputBox);
  // Image upload div delete.
  var deleteBox = document.createElement("img");
  deleteBox.setAttribute("src", "" + Drupal.settings.spider_catalog.delete_png_url + "delete.png");
  deleteBox.setAttribute("style", "cursor:pointer;");
  deleteBox.setAttribute("onclick", "spider_catalog_delete_file('img_brows', 'edit-image-upload-wrapper_" + i + "')");
  inputDiv.appendChild(deleteBox);
  // Image upload div description.
  var description_div = document.createElement("div");
  description_div.setAttribute("class", "description");
  description_div.innerHTML = Drupal.t("Choose image files to upload on save.");
  inputDiv.appendChild(description_div);
  i++;
}
