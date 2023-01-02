<script lang="ts">
	import type Post from "$lib/types/post";
	import type { ComponentType } from "svelte";
	import Header from "./Header.svelte";

    export let Content: ComponentType;
	export let meta: Post;

    let test = typeof Header;

</script>

<h1>{meta.title}</h1>

<svelte:component this={Content}></svelte:component>
