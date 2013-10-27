/**
 * Select theme.
 */
function spider_catalog_set_theme() {
  themeID = document.getElementById('edit-default-themes').value;
  var if_set;
  switch (themeID) {
    case '1':
      if_set = spider_catalog_reset_theme_1();
      if (if_set) {
        document.getElementById("edit-theme-title").value = "new_Default theme";
      }
      break;
  }
}

/**
 * Reset default theme.
 */
function spider_catalog_reset_theme_1() {
  if (confirm(Drupal.t('Do you really whant to reset theme?'))) {
    // Global parametres.
    document.getElementById("edit-rounded-corners-1").checked = true;
    document.getElementById("edit-rounded-corners-0").checked = false;
    document.getElementById("edit-border-style").value = "solid";
    document.getElementById("edit-border-width").value = "1";
    // If jscolor exists.
    if (document.getElementById("edit-background-color").color) {
      document.getElementById("edit-parameters-background-ccolor-1").color.fromString("C9EFFE");
      document.getElementById("edit-parameters-background-ccolor-2").color.fromString("EFF9FD");
      document.getElementById("edit-background-color").color.fromString("F2F2F2");
      document.getElementById("edit-border-color").color.fromString("00AEEF");
      document.getElementById("edit-text-color").color.fromString("20B8F1");
      document.getElementById("edit-color-of-parameter-values").color.fromString("2F699E");
      document.getElementById("edit-hyperlink-color").color.fromString("000000");
      document.getElementById("edit-price-color").color.fromString("FC0303");
      document.getElementById("edit-title-color").color.fromString("FFFFFF");
      document.getElementById("edit-title-background-color").color.fromString("00AEEF");
      document.getElementById("edit-color-of-text-of-buttons").color.fromString("FFFFFF");
      document.getElementById("edit-buttons-background-color").color.fromString("00AEEF");
    }
    else {
      document.getElementById("edit-parameters-background-ccolor-1").value = "C9EFFE";
      document.getElementById("edit-parameters-background-ccolor-2").value = "EFF9FD";
      document.getElementById("edit-background-color").value = "F2F2F2";
      document.getElementById("edit-border-color").value = "00AEEF";
      document.getElementById("edit-text-color").value = "20B8F1";
      document.getElementById("edit-color-of-parameter-values").value = "2F699E";
      document.getElementById("edit-hyperlink-color").value = "000000";
      document.getElementById("edit-price-color").value = "FC0303";
      document.getElementById("edit-title-color").value = "FFFFFF";
      document.getElementById("edit-title-background-color").value = "00AEEF";
      document.getElementById("edit-color-of-text-of-buttons").value = "FFFFFF";
      document.getElementById("edit-buttons-background-color").value = "00AEEF";
    }
    document.getElementById("edit-rating-star-design-1").checked = false;
    document.getElementById("edit-rating-star-design-2").checked = false;
    document.getElementById("edit-rating-star-design-3").checked = false;
    document.getElementById("edit-rating-star-design-4").checked = true;
    document.getElementById("edit-rating-star-design-5").checked = false;
    document.getElementById("edit-category-picture-width").value = "350";
    document.getElementById("edit-category-picture-height").value = "250";
    document.getElementById("edit-category-title-size").value = "22";

    // Cells Page Options.
    document.getElementById("edit-count-of-products-in-the-row").value = "2";
    document.getElementById("edit-count-of-rows-in-the-page").value = "3";
    document.getElementById("edit-product-cell-width").value = "320";
    document.getElementById("edit-product-cell-height").value = "520";
    document.getElementById("edit-picture-width").value = "100";
    document.getElementById("edit-picture-height").value = "100";
    document.getElementById("edit-text-size").value = "12";
    document.getElementById("edit-price-size").value = "20";
    document.getElementById("edit-title-size").value = "20";
    document.getElementById("edit-show-category-1").checked = true;
    document.getElementById("edit-show-category-0").checked = false;
    document.getElementById("edit-show-parameters-1").checked = true;
    document.getElementById("edit-show-parameters-0").checked = false;

    // List Page Options
    document.getElementById("edit-count-of-products-in-the-page-list").value = "10";
    document.getElementById("edit-picture-width-list").value = "100";
    document.getElementById("edit-picture-height-list").value = "100";
    document.getElementById("edit-text-size-list").value = "14";
    document.getElementById("edit-price-size-list").value = "18";
    document.getElementById("edit-show-category-list-1").checked = true;
    document.getElementById("edit-show-category-list-0").checked = false;
    document.getElementById("edit-show-parameters-list-1").checked = true;
    document.getElementById("edit-show-parameters-list-0").checked = false;
    document.getElementById("edit-show-description-list-1").checked = false;
    document.getElementById("edit-show-description-list-0").checked = true;

    // List Product Options
    document.getElementById("edit-picture-width-product").value = "300";
    document.getElementById("edit-picture-height-product").value = "250";
    document.getElementById("edit-text-size-product").value = "12";
    document.getElementById("edit-price-size-product").value = "20";
    document.getElementById("edit-title-size-product").value = "16";
    // If jscolor exists.
    if (document.getElementById("edit-background-color-review").color) {
      document.getElementById("edit-background-color-review").color.fromString("EFF9FD");
      document.getElementById("edit-color-of-review-texts").color.fromString("2F699E");
      document.getElementById("edit-background-color-review-author").color.fromString("C9EFFE");
      document.getElementById("edit-background-color-review-text").color.fromString("EFF9FD");
    }
    else {
      document.getElementById("edit-background-color-review").value = "EFF9FD";
      document.getElementById("edit-color-of-review-texts").value = "2F699E";
      document.getElementById("edit-background-color-review-author").value = "C9EFFE";
      document.getElementById("edit-background-color-review-text").value = "EFF9FD";
    }
    document.getElementById("edit-number-of-reviews-per-page").value = "10";
    return true;
  }
  else {
    return false;
  }
}
